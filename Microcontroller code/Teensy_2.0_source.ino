//Libraries used
#include <SPI.h>
#include <MFRC522.h>

//setup for RFID
#define RST_PIN         4           // Configurable, see typical pin layout above
#define SS_PIN          0           // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

//Setup for one Rotary encoder
#define outputA0 18             // output 1 (CLK) of rotary encoder
#define outputB0 19             // output 2 (DT) of rotary encoder

#define btnTimeout 500          // timeout for the switch

#define outputA1 15             // output 1 (CLK) of rotary encoder
#define outputB1 16             // output 2 (DT) of rotary encoder
#define switch 17               // button (SW) otput of rotary encoder

#define Stats 12
#define Items 13
#define Data 14

//create Variables
String inputString = "";        // a String to hold incoming data
bool stringComplete = false;    // whether the string is complete
int timeout = millis();         // timeout variable for button spam prevention
bool req = false;               // has python initiated a connection?
int now = 0;                    // variable to hold the current millis, used for spam prevention
byte ID_BLOCK = 1;              // Location of the ID on the RFID tag
bool ID = true;                 // variable to prevent double serial output
int a0State = 0;                 // output 1 of Rotary encoder 0
int a0LastState = 0;             // last state of output 1
int a1State = 0;                 // output 1 of Rotary encoder 1
int a1LastState = 0;             // last state of output 1

//SETUP
void setup() {

  // initialize serial:
  Serial.begin(9600);
  // reserve 50 bytes for the inputString:
  inputString.reserve(50);

  // button setup
  pinMode(Stats, INPUT);
  pinMode(Items, INPUT);
  pinMode(Data, INPUT);

  //rotary encoder
  pinMode(switch, INPUT);
  pinMode (outputA0, INPUT);
  pinMode (outputB0, INPUT);
  a0LastState = digitalRead(outputA0);   //setup for rotary encoder

  pinMode (outputA1, INPUT);
  pinMode (outputB1, INPUT);
  a1LastState = digitalRead(outputA1);

  // init RFID (SPI)
  SPI.begin();                  // Init SPI bus
  mfrc522.PCD_Init();           // Init MFRC522 card

}

void loop() {
  buttons();
  wheel0();                     // read rotary encoder0 (scrollwheel)
  wheel1();                     // read rotary encoder 1 (option select)
  readRFID();                   // SERIAL read and RFID

}





void buttons() {
  now = millis();               //get current execution time
  // do something if button has bee pressed
  if (digitalRead(switch) == 0 && now - timeout >= btnTimeout) {

    timeout = millis();
    //Serial.write("button active\n");
    Keyboard.press(KEY_ENTER);
    delay(25);
    Keyboard.release(KEY_ENTER);
  }


  if (digitalRead(Stats) == 0) {
    Keyboard.press(KEY_1);
    delay(25);
    Keyboard.release(KEY_1);
    delay(50);
  } else {}
  if (digitalRead(Items) == 0) {
    Keyboard.press(KEY_2);
    delay(25);
    Keyboard.release(KEY_2);
    delay(50);
  } else {}
  if (digitalRead(Data) == 0) {
    Keyboard.press(KEY_3);
    delay(25);
    Keyboard.release(KEY_3);
    delay(50);
  } else {}

}

void wheel1() {
  a1State = digitalRead(outputA1);
  if (a1State != a1LastState) {
    // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
    if (digitalRead(outputB1) != a1State) {
      //Serial.write("--\n");
      Keyboard.press(KEY_E);
      delay(25);
      Keyboard.release(KEY_E);
    } else if (digitalRead(outputB1) == a1State) {
      //Serial.write("++\n");
      Keyboard.press(KEY_D);
      delay(25);
      Keyboard.release(KEY_D);
    }
    a1LastState = a1State; // Updates the previous state of the outputA with the current state
  }

}
void wheel0() {
  a0State = digitalRead(outputA0);
  if (a0State != a0LastState) {
    // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
    if (digitalRead(outputB0) != a0State) {
      //Serial.write("--\n");
      Keyboard.press(KEY_UP);
      delay(25);
      Keyboard.release(KEY_UP);
    } else if (digitalRead(outputB0) == a0State) {
      //Serial.write("++\n");
      Keyboard.press(KEY_DOWN);
      delay(25);
      Keyboard.release(KEY_DOWN);
    }
    a0LastState = a0State; // Updates the previous state of the outputA with the current state
  }

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
      mfrc522.PCD_Init();
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

      unsigned char buffer1[34];
      MFRC522::StatusCode status;
      byte len;

      if (ID) {
        Serial.println("req_write-WRITE:START");
        ID = false;
      }
      Serial.setTimeout(20000L) ;
      len = Serial.readBytesUntil('#', (char *) buffer1, 30) ;
      for (byte i = len; i < 30; i++) buffer1[i] = ' ';     // pad with spaces

      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, ID_BLOCK, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
        Serial.println("req_write-ERROR:AUTH_FAIL");
        return;
      }
      // Write block
      status = mfrc522.MIFARE_Write(ID_BLOCK, buffer1, 16);
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
      mfrc522.PCD_Init();
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

      byte buffer1[32];
      byte len = sizeof(buffer1);
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
          if (buffer1[i] != '\n' && buffer1[i] != ' ') {
            ans += char(buffer1[i]);
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
