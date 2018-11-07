//Libraries used
#include <SPI.h>
#include <MFRC522.h>

//setup for RFID
#define RST_PIN         4
#define SS_PIN          0
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

//Setup for Rotary encoders
#define outputA0 5             // output 1 (CLK) of rotary encoder 0 
#define outputB0 6             // output 2 (DT) of rotary encoder 0 

#define btnTimeout 500         // timeout for the switch

#define outputA1 7             // output 1 (CLK_0) of rotary encodern 1
#define outputB1 8             // output 2 (DT_0) of rotary encoder 1
#define switch 9               // button (SW) otput of rotary encoder

#define outputA2 22            // output 1 (CLK_1) of rotary encoder 1
#define outputB2 23            // output 1 (CLK_1) of rotary encoder 1

//tab selector buttons
#define Stats 11
#define Stats_LED 14
#define Items 12
#define Items_LED 15
#define Data 13
#define Data_LED 16

//rotary switch
int rot_switch[5] = {
  17,
  18,
  19,
  20,
  21
};

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
bool con_init = false;
int pos_state[5] = {0, 0, 0, 0, 0};
int pos_prev[5] = {0, 0, 0, 0, 0};

//SETUP
void setup() {

  // initialize serial:
  Serial.begin(9600);
  // reserve 50 bytes for the inputString:
  inputString.reserve(50);

  //rotary switch array
  for (int i = 0; i < sizeof(rot_switch); i++) {
    pinMode(i, INPUT);
  }


  // button setup
  pinMode(Stats, INPUT);
  pinMode(Stats_LED, OUTPUT);

  pinMode(Items, INPUT);
  pinMode(Items_LED, OUTPUT);

  pinMode(Data, INPUT);
  pinMode(Data_LED, OUTPUT);

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
  int thing[3] = {0, 0, 0};
  buttons(thing);             // read buttons and handle LEDs
  wheel0();                     // read rotary encoder0 (scrollwheel)
  wheel1();                     // read rotary encoder 1 (option select)
  readRFID();                   // SERIAL read and RFID
  rotarySwitch();
}
void rotarySwitch() {
  for(int i=0;i<=4;i++){
    pos_state[i] = digitalRead(rot_switch[i]);
    if(pos_state[i]==pos_prev[i]){
      continue;
    }else{
      
      pos_prev[i]=pos_state[i];
      if(pos_state[i]==0){
        //Serial.println((char)i+4);
        Keyboard.print((char)i+4);
      }
    }
  }
}

void buttons(int btn[3]) {
  now = millis();               //get current execution time
  // do something if button has bee pressed
  if (digitalRead(switch) == 0 && now - timeout >= btnTimeout) {

    timeout = millis();
    //Serial.write("button active\n");
    Keyboard.press(KEY_ENTER);
    delay(25);
    Keyboard.release(KEY_ENTER);
  }


  if ((digitalRead(Stats) == 0 || btn[0] == 1) && now - timeout >= btnTimeout) {
    digitalWrite(Items_LED, 0);
    digitalWrite(Stats_LED, 1);
    digitalWrite(Data_LED, 0);
    if (btn[0] == 0) {
      Keyboard.press(KEY_1);
      delay(25);
      Keyboard.release(KEY_1);
      delay(50);
      timeout = millis();
    }
  }
  if ((digitalRead(Items) == 0 || btn[1] == 1) && now - timeout >= btnTimeout) {
    digitalWrite(Items_LED, 1);
    digitalWrite(Stats_LED, 0);
    digitalWrite(Data_LED, 0);
    if (btn[1] == 0) {
      Keyboard.press(KEY_2);
      delay(25);
      Keyboard.release(KEY_2);
      delay(50);
      timeout = millis();
    }
  }
  if ((digitalRead(Data) == 0 || btn[2] == 1) && now - timeout >= btnTimeout) {
    digitalWrite(Items_LED, 0);
    digitalWrite(Stats_LED, 0);
    digitalWrite(Data_LED, 1);
    if (btn[2] == 0) {
      Keyboard.press(KEY_3);
      delay(25);
      Keyboard.release(KEY_3);
      delay(50);
      timeout = millis();
    }
  }

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
    //Serial.println(strtok(char(inputString*),"-"));

    if (inputString == "con_test") {
      Serial.println("success");
      inputString = "";
      stringComplete = false;
    }
    if (inputString == "con_init") {
      con_init = true;
      Serial.println("Ready");
      inputString = "";
      stringComplete = false;
    }
    if (con_init && inputString.indexOf("con_setup") != -1) {
      char selected[1] = {inputString[inputString.indexOf(":") + 1]};
      int sel[3][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
      };
      buttons(sel[atoi(selected)]);
      Serial.println("con_setup-SETUP:SUCCESS");
      con_init = false;
      inputString = "";
      stringComplete = false;
    }
    if (inputString == "con_start" && !con_init) {
      Serial.println("Ready");
      req = true;
      inputString = "";
      stringComplete = false;
    }
    if (inputString == "req_write" && req && !con_init) {
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
    if (inputString == "req_ID" && req && !con_init) {
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
    if (inputString == "con_exit") {
      if (con_init) {
        con_init = false;
        Serial.write("con_exit-SUCCESS:CON_END\n");
      }
      if (req) {
        req = false;
        Serial.write("con_exit-SUCCESS:CON_END\n");
      }

      inputString = "";
      stringComplete = false;
    }
    inputString = "";
    stringComplete = false;
  }
}
