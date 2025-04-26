#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "DFPlayerTD5580ATest "
#define VERSION "V0.3 "
#define DEVICE_UNDER_TEST "PMD: LCD and DFPlayer"  //A PMD model number
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "USA"

/*
   File: DFPlayerTD5580ATest
   Author: Forrest Lee Erickson
   Date: 20240929
   Date: 20250414 V0.2, added functionality from :https://github.com/DFRobot/DFRobotDFPlayerMini/blob/master/examples/FullFunction/FullFunction.ino

*/

//OTA Stuff
/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-ota-elegantota-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files. The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Softwar
*********/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>

const char* ssid = "VRX";
const char* password = "textinsert";

AsyncWebServer server(80);



#define BAUDRATE 115200
#define BAUD_DFPLAYER 9600


#include <DFRobotDFPlayerMini.h>

// Create a SoftwareSerial object with RX and TX pins
//SoftwareSerial mySerial(16, 17); // RX, TX

HardwareSerial mySerial1(2);  // Use UART2
DFRobotDFPlayerMini dfPlayer;

const int LED_PIN = 13;  // Krake

//const int LED_PIN = 2;  // ESP32 LED
const int LED_D6 = 23;  // ESP32 GPIO23 pin 16 on kit.

const int nDFPlayer_BUSY = 4; //Busy from DFPLayer

const int trac1 = 1;
const int trac2 = 2;
#define TXD2 17
#define RXD2 16

bool isDFPlayerDetected = false;

//Functions
void setupDFPlayer() {
  //  Setup UART for DFPlayer
  Serial.println("UART2 Begin");
  mySerial1.begin(BAUD_DFPLAYER, SERIAL_8N1, RXD2, TXD2);
  while (!mySerial1) {
    ;  // wait for DFPlayer serial port to connect.
  }
  // Essential Initialize of DFPlayer Mini
  //  Serial.println("DFPlayer begin with ACK and Reset");

  //  dfPlayer.begin(mySerial1, true, true);  // (Stream &stream, bool isACK, bool doReset)
  // dfPlayer.begin(mySerial1, true, false);  // (Stream &stream, bool isACK, bool doReset)
  // dfPlayer.reset();

  // delay(3000);

  Serial.println("Begin DFPlayer isACK true, doReset false.");
  //  if (!dfPlayer.begin(mySerial1, true, true)) {
  if (!dfPlayer.begin(mySerial1, true, false)) {
    Serial.println("DFPlayer Mini not detected or not working.");
    Serial.println("Check for missing SD Card.");
    //while (true)
    isDFPlayerDetected = false;
    ;  // Stop execution
  } else {
    isDFPlayerDetected = true;
    Serial.println("DFPlayer Mini detected!");
  }

  //  delay(3000);  //Required for volum to set
  // Set volume (0 to 30)
  dfPlayer.volume(20);  // Set initial volume max
                        //  dfPlayer.volume(20);       // Set initial volume low
  // delay(3000);
  // Serial.print("Volume is set to: ");
  // digitalWrite(LED_D6, HIGH);             //Start of volume read.
  // Serial.println(dfPlayer.readVolume());  //Causes program lock up
  // digitalWrite(LED_D6, LOW);              //End of volume read.

  dfPlayer.setTimeOut(500);  // Set serial communictaion time out 500ms
  delay(100);

  Serial.print("dfPlayer State: ");
  Serial.println(dfPlayer.readState());  //read mp3 state
  Serial.print("dfPlayer Volume: ");
  Serial.println(dfPlayer.readVolume());  //read current volume
  Serial.print("dfPlayer EQ: ");
  Serial.println(dfPlayer.readEQ());                   //read EQ setting
  Serial.print("SD Card FileCounts: ");
  Serial.println(dfPlayer.readFileCounts());           //read all file counts in SD card
  Serial.print("Current File Number: ");
  Serial.println(dfPlayer.readCurrentFileNumber());    //read current play file number
  Serial.print("File Counts In Folder: ");
  Serial.println(dfPlayer.readFileCountsInFolder(3));  //read file counts in folder SD:/03

  //  dfPlayer.EQ(0);          // Normal equalization //Causes program lock up


}  //setupDFPLayer

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
}//end rintDetail

void dfPlayerUpdate(void){
   static unsigned long timer = millis();
  
//  if (millis() - timer > 3000) {
  if (millis() - timer > 10000) {
    timer = millis();
    dfPlayer.next();  //Play next mp3 every 3 second.
  }
  
  if (dfPlayer.available()) {
    printDetail(dfPlayer.readType(), dfPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}// end

void dfPlayerNotBusy(){
  //If not busy play next.
  //if (digitalRead(4)){  //busy pin.
  // if (digitalRead(nDFPlayer_BUSY)){  //busy pin.
  //   dfPlayer.next();
  // }
  

   if (dfPlayer.available()) {
    printDetail(dfPlayer.readType(), dfPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
    if(DFPlayerPlayFinished == dfPlayer.readType() ){
       dfPlayer.next();
    }
  }

}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  pinMode(LED_D6, OUTPUT);
  digitalWrite(LED_D6, LOW);  //Start out low


  // Start serial communication
  Serial.begin(BAUDRATE);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB
  }
  //delay(500);
  Serial.println("===================================");
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier
  Serial.println("===================================");
  Serial.println();

  setupOTA();
  Serial.print("OTA setup done.");


  setupDFPlayer();
  Serial.print("DFPlayer setup done.");

  Serial.println("End of setup");
  digitalWrite(LED_PIN, LOW);
}  // end of setup()

void playNotBusy(){
  boolean play_state = digitalRead(4);
  if(play_state == HIGH){
    //mp3_next ();
    dfPlayer.next();
    delay(1000);
  }
  delay(1000);
}


void loop() {

ElegantOTA.loop();

dfPlayerUpdate();

//playNotBusy();
//dfPlayerNotBusy();

digitalWrite(LED_PIN, digitalRead(nDFPlayer_BUSY));

  // Serial.println("Play a track");
  // dfPlayer.play(0);
  // delay(1000);
  // dfPlayer.play(1);
  // delay(1000);
  // dfPlayer.play(2);
  // delay(1000);

  //Not working
  // Serial.println("Play MP3 folder");
  // dfPlayer.playMp3Folder(1);
  // delay(2000);

}  // end of loop()
