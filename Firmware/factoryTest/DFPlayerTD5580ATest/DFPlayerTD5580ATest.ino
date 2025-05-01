#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "DFPlayerTD5580ATest "
#define VERSION "V0.6 "
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

// const char* ssid = "Hometplink";
// const char* password = "adt@1963#";

AsyncWebServer server(80);

#define BAUDRATE 115200  // For UART0 the DEBUG Serial Monitor

#define BAUD_DFPLAYER 9600  //for UART2 to the DFPlayer
#define TXD2 17
#define RXD2 16

#include <DFRobotDFPlayerMini.h>

DFRobotDFPlayerMini dfPlayer;

HardwareSerial mySerial1(2);  // Use UART2

const int LED_PIN = 13;  // Krake
//const int LED_PIN = 2;  // ESP32 LED

const int nDFPlayer_BUSY = 4;  //not Busy from DFPLayer
bool isDFPlayerDetected = false;
int volumeDFPlayer = 20;    // Set volume initial volume. Range is [0 to 30]

//Functions
void setupDFPlayer() {
  //  Setup UART for DFPlayer
  Serial.println("UART2 Begin");
  mySerial1.begin(BAUD_DFPLAYER, SERIAL_8N1, RXD2, TXD2);
  while (!mySerial1) {
    Serial.println("UART2 inot initilaized.");
    delay(1000);
    ;  // wait for DFPlayer serial port to connect.
  }
  Serial.println("Begin DFPlayer: isACK true, doReset false.");
  if (!dfPlayer.begin(mySerial1, true, false)) {
    Serial.println("DFPlayer Mini not detected or not working.");
    Serial.println("Check for missing SD Card.");
    isDFPlayerDetected = false;
  } else {
    isDFPlayerDetected = true;
    Serial.println("DFPlayer Mini detected!");
  }

  dfPlayer.volume(volumeDFPlayer);  // Set initial volume
  dfPlayer.setTimeOut(500);  // Set serial communictaion time out 500ms
  delay(100);  //Todo, ?? necessary for DFPlayer processing

  Serial.print("=================");
  Serial.print("dfPlayer State: ");
  Serial.println(dfPlayer.readState());  //read mp3 state
  Serial.print("dfPlayer Volume: ");
  Serial.println(dfPlayer.readVolume());  //read current volume
  Serial.print("dfPlayer EQ: ");
  Serial.println(dfPlayer.readEQ());  //read EQ setting
  Serial.print("SD Card FileCounts: ");
  Serial.println(dfPlayer.readFileCounts());  //read all file counts in SD card
  Serial.print("Current File Number: ");
  Serial.println(dfPlayer.readCurrentFileNumber());  //read current play file number
  Serial.print("File Counts In Folder: ");
  Serial.println(dfPlayer.readFileCountsInFolder(3));  //read file counts in folder SD:/03

  //  dfPlayer.EQ(0);          // Normal equalization //Causes program lock up

  Serial.print("=================");

}  //setupDFPLayer

void printDetail(uint8_t type, int value) {
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

}  //end rintDetail

void dfPlayerUpdate(void) {
  static unsigned long timer = millis();

  if (millis() - timer > 3000) {
    //  if (millis() - timer > 10000) {
    timer = millis();
    dfPlayer.next();  //Play next mp3 every 3 second.
  }

  if (dfPlayer.available()) {
    printDetail(dfPlayer.readType(), dfPlayer.read());  //Print the detail message from DFPlayer to handle different errors and states.
  }
}  // end

void dfPlayerNotBusy() {
  //If not busy play next.
  //  if (digitalRead(4)){  //busy pin.
  // if (digitalRead(nDFPlayer_BUSY)) {  //busy pin.
  //   dfPlayer.next();
  // }
  Serial.println("Testing if DFPlayer is Busy and DFPlayer.available");

//  if ( (true == !digitalRead(nDFPlayer_BUSY) ) && (true == dfPlayer.available()) )  {
//  if ( (true ) && (true == dfPlayer.available()) )  {
  if ( (true == digitalRead(nDFPlayer_BUSY) ) && (true ) )  {
    //delay(1000);
    Serial.println("Testing if DFPlayer NOT Busy and DFPlayer.available");
    printDetail(dfPlayer.readType(), dfPlayer.read());  //Print the detail message from DFPlayer to handle different errors and states.
//    if (DFPlayerPlayFinished == dfPlayer.readType()) {
    if (true ) {
//      delay(1000);
      dfPlayer.next();
    }
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);


  pinMode(nDFPlayer_BUSY, INPUT_PULLUP);

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

void playNotBusy() {
  //Plays all files succsivly.
  boolean play_state = digitalRead(nDFPlayer_BUSY);
  if (play_state == HIGH) {
    //mp3_next ();
    dfPlayer.next();
  }
  delay(1000); //This should be removed from code. We set a time for the next allowed message.
  // Or better yet use interupt to find the end of BUSY and set time for the next allowed DFPlayer message
}


void loop() {

  ElegantOTA.loop();

  //dfPlayerUpdate();  //Play all but for only a constant time.

  playNotBusy();
  //dfPlayerNotBusy();  // Play all to end

  digitalWrite(LED_PIN, !digitalRead(nDFPlayer_BUSY));  //Polarity of the LED must be inverted relative to the BUSY.

  // Serial.println("Play a track");
  // dfPlayer.play(0);
  // delay(1000);
  // dfPlayer.play(1);
  // delay(1000);
  // dfPlayer.play(2);
  // delay(1000);

}  // end of loop()
