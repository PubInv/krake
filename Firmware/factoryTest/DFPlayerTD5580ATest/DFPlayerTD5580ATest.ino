#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "DFPlayerTD5580ATest "
#define VERSION "V0.7 "
#define DEVICE_UNDER_TEST "Krake: DFPlayer and OTA"  //A PMD model number
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "USA"

/*
   File: DFPlayerTD5580ATest
   Author: Forrest Lee Erickson
   Date: 20240929
   Date: 20250414 V0.2, added functionality from :https://github.com/DFRobot/DFRobotDFPlayerMini/blob/master/examples/FullFunction/FullFunction.ino

*/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>

const char* ssid = "VRX";
const char* password = "textinsert";

// const char* ssid = "Hometplink";
// const char* password = "adt@1963#";

//Houstin network
// const char* ssid = "DOS_WIFI";
// const char* password = "$Suve07$$";

// Austin network
// const char* ssid = "readfamilynetwork";
// const char* password = "magicalsparrow96";


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
int volumeDFPlayer = 20;  // Set volume initial volume. Range is [0 to 30]
int numberFilesDF = 0;    // Number of the audio files found.

//Functions
void setupDFPlayer() {
  //  Setup UART for DFPlayer
  Serial.println("UART2 Begin");
  mySerial1.begin(BAUD_DFPLAYER, SERIAL_8N1, RXD2, TXD2);
  while (!mySerial1) {
    Serial.println("UART2 inot initilaized.");
    delay(100);
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
  dfPlayer.setTimeOut(500);         // Set serial communictaion time out 500ms
  //  delay(100);

  dfPlayer.start();  //Todo, ?? necessary for DFPlayer processing
  delay(1000);
  dfPlayer.play(11);  //DFPlayer Splash
  delay(100);
  dfPlayer.stop();
  delay(1000);
  dfPlayer.previous();
  delay(1500);
  dfPlayer.play();  //DFPlayer Splash

  displayDFPlayerStats();

}  //setupDFPLayer

void displayDFPlayerStats() {
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
  numberFilesDF = dfPlayer.readCurrentFileNumber();
  Serial.println(numberFilesDF);  //Display the read current play file number
  Serial.print("File Counts In Folder: ");
  Serial.println(dfPlayer.readFileCountsInFolder(3));  //read file counts in folder SD:/03
  //  dfPlayer.EQ(0);          // Normal equalization //Causes program lock up
  Serial.print("=================");
}
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
}  //end printDetail for DFPlayer


void dfPlayerUpdate(void) {
  unsigned long timePlay = 3000;  //Plays 3 seconds of all files.
  static unsigned long timer = millis();
  if (millis() - timer > timePlay) {
    //  if (millis() - timer > 10000) {
    timer = millis();
    dfPlayer.next();  //Play next mp3 every 3 second.
  }
  if (dfPlayer.available()) {
    printDetail(dfPlayer.readType(), dfPlayer.read());  //Print the detail message from DFPlayer to handle different errors and states.
  }
}  // end




// Functions to learn DFPlayer behavior
void playNotBusy() {
  //Plays all files succsivly.
  Serial.println("PlayNotBusy");
  if (HIGH == digitalRead(nDFPlayer_BUSY)) {
    //mp3_next ();
    dfPlayer.next();
  }
  if (dfPlayer.available()) {
    printDetail(dfPlayer.readType(), dfPlayer.read());  //Print the detail message from DFPlayer to handle different errors and states.
  }
  delay(1000);  //This should be removed from code. We set a time for the next allowed message.
  // Or better yet use interupt to find the end of BUSY and set time for the next allowed DFPlayer message
}  // end playNotBusy



// Play a track but not if the DFPlayer is busy
bool playAlarmLevel(int alarmNumberToPlay) {
  static unsigned long timer = millis();

  const unsigned long delayPlayLevel = 20;
  //  const int MAX_ALARM_NUMBER = 6;
  int tracNumber = alarmNumberToPlay;

  if (millis() - timer > delayPlayLevel) {
    //  if (millis() - timer > 10000) {
    timer = millis();

    //If not busy play the alarm message
    if ((0 > tracNumber < 0) || (numberFilesDF < tracNumber)) {
      return false;
    } else                                        //Valid track number
      if (HIGH == digitalRead(nDFPlayer_BUSY)) {  // Should the test for busy be at the start of this function???
        //mp3_next ();
        dfPlayer.play(tracNumber);
      } else {
        Serial.println("Not done playing previous file");
      }
    if (dfPlayer.available()) {
      printDetail(dfPlayer.readType(), dfPlayer.read());  //Print the detail message from DFPlayer to handle different errors and states.
    }
    return true;
  }
}  // end of playAlarmLevel


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
  serialSplash();

  //DFPlayer Splash
  setupDFPlayer();
  //  dfPlayer.play(9);  //DFPlayer Splash

  setupOTA();
  Serial.print("OTA setup done.");

  while (LOW == digitalRead(nDFPlayer_BUSY)) {
    ;  //Hold still till splash file plays
  }

  Serial.println("End of setup");
  digitalWrite(LED_PIN, LOW);
}  // end of setup()

void loop() {
  ElegantOTA.loop();
  //dfPlayerUpdate();  //Play all but for only a constant time.
  //  playNotBusy();  // Play all but  only when not busy.
  //playAlarmLevel(3);
  digitalWrite(LED_PIN, !digitalRead(nDFPlayer_BUSY));  //Polarity of the LED must be inverted relative to the BUSY.

  checkSerial();
}  // end of loop()
