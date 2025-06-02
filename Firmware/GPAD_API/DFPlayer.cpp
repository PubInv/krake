
#include "DFPlayer.h"
#include "gpad_utility.h"
#include <DFRobotDFPlayerMini.h>
DFRobotDFPlayerMini dfPlayer;

HardwareSerial mySerial1(2);  // Use UART2

const int LED_PIN = 13;  // Krake
//const int LED_PIN = 2;  // ESP32 LED

const int nDFPlayer_BUSY = 4;  //not Busy from DFPLayer
bool isDFPlayerDetected = false;
int volumeDFPlayer = 20;  // Set volume initial volume. Range is [0 to 30]
int numberFilesDF = 0;    // Number of the audio files found.



// The serial command system is from: https://www.dfrobot.com/blog-1462.html?srsltid=AfmBOoqm5pHrLswInrTwZ9vcYdxxPC_zH2zXnoro2FyTLEL4L57IW3Sn

char command;
int pausa = 0;


void serialSplashDFP() {
  Serial.println("===================================");
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(PROG_NAME);
  Serial.println(FIRMWARE_VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier
  Serial.println("===================================");
  Serial.println();
}

void menu_opcoes() {
  Serial.println();
  Serial.println(F("=================================================================================================================================="));
  Serial.println(F("Commands:"));
  Serial.println(F(" [1-3] To select the MP3 file"));
  Serial.println(F(" [s] stopping reproduction"));
  Serial.println(F(" [p] pause/continue music"));
  Serial.println(F(" [+ or -] increases or decreases the volume"));
  Serial.println(F(" [< or >] forwards or backwards the track"));
  Serial.println();
  Serial.println(F("================================================================================================================================="));
}  //end menu_opcoes()


void checkSerial(void) {

  //Waits for data entry via serial
  while (Serial.available() > 0) {
    command = Serial.read();

    if ((command >= '1') && (command <= '9')) {
      Serial.print("Music reproduction");
      Serial.println(command);
      command = command - 48;
      dfPlayer.play(command);
      menu_opcoes();
    }

    //Reproduction
    //Stop

    if (command == 's') {
      dfPlayer.stop();
      Serial.println("Music Stopped!");
      menu_opcoes();
    }

    //Pausa/Continua a musica
    if (command == 'p') {
      pausa = !pausa;
      if (pausa == 0) {
        Serial.println("Continue...");
        dfPlayer.start();
      }

      if (pausa == 1) {
        Serial.println("Music Paused!");
        dfPlayer.pause();
      }

      menu_opcoes();
    }


    //Increases volume
    if (command == '+') {
      dfPlayer.volumeUp();
      Serial.print("Current volume:");
      Serial.println(dfPlayer.readVolume());
      menu_opcoes();
    }

    if (command == '<') {
      dfPlayer.previous();
      Serial.println("Previous:");
      Serial.print("Current track:");
      Serial.println(dfPlayer.readCurrentFileNumber() - 1);
      menu_opcoes();
    }

    if (command == '>') {
      dfPlayer.next();
      Serial.println("next:");
      Serial.print("Current track:");
      Serial.println(dfPlayer.readCurrentFileNumber() + 1);
      menu_opcoes();
    }

    //Decreases volume
    if (command == '-') {
      dfPlayer.volumeDown();
      Serial.print("Current Volume:");
      Serial.println(dfPlayer.readVolume());
      menu_opcoes();
    }
  }
}  // end checkSerial

//Functions
void setupDFPlayer() {
// Debatably, this this should be in the GPAD_HAL, not here....but
// it is modular to place it here.
  pinMode(nDFPlayer_BUSY, INPUT_PULLUP);


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
//  dfPlayer.play(11);  //DFPlayer Splash
  dfPlayer.play(9);
  delay(100);
  dfPlayer.stop();
  delay(1000);
  dfPlayer.previous();
  delay(1500);
  dfPlayer.play();  //DFPlayer Splash

  displayDFPlayerStats();

}  //setupDFPLayer


void setVolume(int zeroToThirty) {
  dfPlayer.volume(volumeDFPlayer);
}


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

void playNotBusyLevel(int level) {
  //Plays all files succsivly.
  Serial.println("PlayNotBusyLevel");
  if (HIGH == digitalRead(nDFPlayer_BUSY)) {
    //mp3_next ();
    // Note....we should in fact use file names, not numbers here,
    // or must at least build a data structure to associate the two.
    // that shoulde be future work.
    dfPlayer.play(level+1);
    Serial.println("HIGH .next called! =================");
  } 
  if (dfPlayer.available()) {
    printDetail(dfPlayer.readType(), dfPlayer.read());  //Print the detail message from DFPlayer to handle different errors and states.
  }
  delay(1000);  //This should be removed from code. We set a time for the next allowed message.
  // Or better yet use interupt to find the end of BUSY and set time for the next allowed DFPlayer message
}


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
  } else {
    return false;
  }
}  // end of playAlarmLevel


// void setupDFP() {
//   pinMode(LED_PIN, OUTPUT);
//   digitalWrite(LED_PIN, HIGH);
//   pinMode(nDFPlayer_BUSY, INPUT_PULLUP);

//   // Start serial communication
//   Serial.begin(BAUDRATE);
//   while (!Serial) {
//     ;  // wait for serial port to connect. Needed for native USB
//   }

//   //delay(500);
//   serialSplash();

//   //DFPlayer Splash
//   setupDFPlayer();
//   //  dfPlayer.play(9);  //DFPlayer Splash

//   setupOTA();
//   Serial.print("OTA setup done.");

//   while (LOW == digitalRead(nDFPlayer_BUSY)) {
//     ;  //Hold still till splash file plays
//   }

//   Serial.println("End of setup");
//   digitalWrite(LED_PIN, LOW);
// }  // end of setup()

