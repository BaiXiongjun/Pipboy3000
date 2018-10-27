
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

#define outputA0 2
#define outputB0 3
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
int timeout = millis();
bool req = false;
int now = 0;
byte ID_BLOCK = 1;
bool ID = true;
int aState = 0;
int aLastState = 0;
void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 50 bytes for the inputString:
  inputString.reserve(50);
  pinMode(7, INPUT);
  pinMode (outputA0, INPUT);
  pinMode (outputB0, INPUT);
  aLastState = digitalRead(outputA0);

  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card

}

void loop() {

  now = millis();

  if (digitalRead(7) == 0 && now - timeout >= 1000) {
    timeout = millis();
    Serial.write("button active\n");
  }
  wheel();
  readRFID();

}

void wheel() {
  aState = digitalRead(outputA0);
  if (aState != aLastState) {
    // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
    if (digitalRead(outputB0) != aState) {
      //Serial.write("--\n");
    } else {
      //Serial.write("++\n");
    }
  }
  aLastState = aState; // Updates the previous state of the outputA with the current state
}

void readRFID() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    //Serial.write(inChar);
    if (inChar != '\n') {
      inputString += inChar;
    }
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  // print the string when a newline arrives:

  if (stringComplete) {
    //Serial.print(inputString + " " + req);
    digitalWrite(13, 1);
    if (inputString == "con_test") {
      Serial.println("success");
      inputString = "";
      stringComplete = false;
    }
    if (inputString == "con_start") {
      Serial.println("Ready");
      req = true;
      inputString = "";
      stringComplete = false;
    }
    if (inputString == "req_write" && req) {
      MFRC522::MIFARE_Key key;
      for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

      // Look for new cards
      if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return;
      }

      // Select one of the cards
      if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
      }
      MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);

      byte buffer[34];
      MFRC522::StatusCode status;
      byte len;

      if (ID) {
        Serial.println("req_write-WRITE:START");
        ID = false;
      }
      Serial.setTimeout(20000L) ;
      len = Serial.readBytesUntil('#', (char *) buffer, 30) ;
      for (byte i = len; i < 30; i++) buffer[i] = ' ';     // pad with spaces

      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, ID_BLOCK, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
        Serial.println("req_write-ERROR:AUTH_FAIL");
        return;
      }
      // Write block
      status = mfrc522.MIFARE_Write(ID_BLOCK, buffer, 16);
      if (status != MFRC522::STATUS_OK) {
        Serial.println("req_write-ERROR:WRITE_FAIL");
        return;
      }
      if (status == MFRC522::STATUS_OK) {
        Serial.println("req_write-SUCCESS:DONE");
        ID = true;
      }
      inputString = "";
      stringComplete = false;

    }
    if (inputString == "req_ID" && req) {
      if (ID) {
        Serial.println("req_ID-READ:START");
        ID = false;
      }
      MFRC522::MIFARE_Key key;
      MFRC522::StatusCode status;
      for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

      if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return;
      }
      if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
      }

      char buffer1[32];
      byte len = 32;
      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, ID_BLOCK, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
      if (status != MFRC522::STATUS_OK) {
        Serial.println("req_ID-ERROR:AUTH_FAIL");
        return;
      }
      status = mfrc522.MIFARE_Read(ID_BLOCK, buffer1, &len);
      if (status != MFRC522::STATUS_OK) {
        Serial.println("req_ID-ERROR:READ_FAIL");
        //Serial.println(mfrc522.GetStatusCodeName(status));
        return;
      }
      if (status == MFRC522::STATUS_OK) {
        String ans = "req_ID-SUCCESS:";
        for (uint8_t i = 1; i < 16; i++) {
          if (buffer1[i] != '\n' && buffer1 != 0x20) {
            ans += buffer1[i];
          }
        }
        Serial.println(ans);
        req = false;
        ID = true;
        inputString = "";
        stringComplete = false;
      }
    }
    if (inputString == "req_exit" && req) {
      req = false;
      Serial.write("req_exit-SUCCESS:CON_END\n");
      inputString = "";
      stringComplete = false;
    }

  }
  }
