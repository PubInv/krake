
#include "DFPlayer.h"
#include "gpad_utility.h"
#include <DFRobotDFPlayerMini.h>
DFRobotDFPlayerMini dfPlayer;

HardwareSerial mySerial1(2); // Use UART2

const int LED_PIN = 13; // Krake
// const int LED_PIN = 2;  // ESP32 LED

const int nDFPlayer_BUSY = 4; // not Busy from DFPLayer
bool isDFPlayerDetected = false;
int volumeDFPlayer = 20; // Set volume initial volume. Range is [0 to 30]
int numberFilesDF = 0;   // Number of the audio files found.

// The serial command system is from: https://www.dfrobot.com/blog-1462.html?srsltid=AfmBOoqm5pHrLswInrTwZ9vcYdxxPC_zH2zXnoro2FyTLEL4L57IW3Sn

char command;
int pausa = 0;

void serialSplashDFP()
{
  Serial.println("===================================");
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(PROG_NAME);
  Serial.println(FIRMWARE_VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__)); // compile date that is used for a unique identifier
  Serial.println("===================================");
  Serial.println();
}

void menu_opcoes()
{
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
} // end menu_opcoes()

void checkSerial(void)
{
  if (!isDFPlayerDetected) return;

  // Waits for data entry via serial
  while (Serial.available() > 0)
  {
    command = Serial.read();

    if ((command >= '1') && (command <= '9'))
    {
      Serial.print("Music reproduction");
      Serial.println(command);
      command = command - 48;
      dfPlayer.play(command);
      menu_opcoes();
    }

    // Reproduction
    // Stop

    if (command == 's')
    {
      dfPlayer.stop();
      Serial.println("Music Stopped!");
      menu_opcoes();
    }

    // Pausa/Continua a musica
    if (command == 'p')
    {
      pausa = !pausa;
      if (pausa == 0)
      {
        Serial.println("Continue...");
        dfPlayer.start();
      }

      if (pausa == 1)
      {
        Serial.println("Music Paused!");
        dfPlayer.pause();
      }

      menu_opcoes();
    }

    // Increases volume
    if (command == '+')
    {
      dfPlayer.volumeUp();
      Serial.print("Current volume:");
      Serial.println(dfPlayer.readVolume());
      menu_opcoes();
    }

    if (command == '<')
    {
      dfPlayer.previous();
      Serial.println("Previous:");
      Serial.print("Current track:");
      Serial.println(dfPlayer.readCurrentFileNumber() - 1);
      menu_opcoes();
    }

    if (command == '>')
    {
      dfPlayer.next();
      Serial.println("next:");
      Serial.print("Current track:");
      Serial.println(dfPlayer.readCurrentFileNumber() + 1);
      menu_opcoes();
    }

    // Decreases volume
    if (command == '-')
    {
      dfPlayer.volumeDown();
      Serial.print("Current Volume:");
      Serial.println(dfPlayer.readVolume());
      menu_opcoes();
    }
  }
} // end checkSerial

// Functions
void setupDFPlayer()
{
  // DFPlayer BUSY pin is usually LOW while playing and HIGH when idle.
  pinMode(nDFPlayer_BUSY, INPUT_PULLUP);

  Serial.println("UART2 Begin");
  mySerial1.begin(BAUD_DFPLAYER, SERIAL_8N1, RXD2, TXD2);
  delay(1000); // Give the DFPlayer and SD card time to boot.

  // ACK=true can make some DFPlayer clones unstable or silent.
  // Use ACK=false for the most compatible startup behavior.
  Serial.println("Begin DFPlayer: isACK false, doReset false.");
  if (!dfPlayer.begin(mySerial1, false, false))
  {
    Serial.println("DFPlayer Mini not detected or not working.");
    Serial.println("Check wiring, common GND, SD card, and file names such as 0001.mp3.");
    isDFPlayerDetected = false;
    return;
  }

  isDFPlayerDetected = true;
  Serial.println("DFPlayer Mini detected!");

  dfPlayer.setTimeOut(500);
  delay(500);

  dfPlayer.volume(volumeDFPlayer); // Range: 0 to 30
  delay(300);

  numberFilesDF = dfPlayer.readFileCounts();
  Serial.print("SD Card FileCounts: ");
  Serial.println(numberFilesDF);

  if (numberFilesDF <= 0)
  {
    Serial.println("No MP3 files found. Format SD as FAT32 and name files 0001.mp3, 0002.mp3, etc.");
    return;
  }

  // Startup audio test: play track 1 and do NOT stop it immediately.
  // The old code played track 9, stopped after 100 ms, then changed tracks,
  // which could make the device appear silent.
  Serial.println("DFPlayer startup test: playing track 1 for 5 seconds...");
  dfPlayer.play(1);
  delay(5000);

  displayDFPlayerStats();
} // setupDFPlayer

void setVolume(int zeroToThirty)
{
  volumeDFPlayer = constrain(zeroToThirty, 0, 30);
  dfPlayer.volume(volumeDFPlayer);
}

void displayDFPlayerStats()
{
  Serial.println("=================");
  Serial.print("dfPlayer State: ");
  Serial.println(dfPlayer.readState()); // read mp3 state
  Serial.print("dfPlayer Volume: ");
  Serial.println(dfPlayer.readVolume()); // read current volume
  Serial.print("dfPlayer EQ: ");
  Serial.println(dfPlayer.readEQ()); // read EQ setting
  Serial.print("SD Card FileCounts: ");
  numberFilesDF = dfPlayer.readFileCounts();
  Serial.println(numberFilesDF); // save actual SD file count
  Serial.print("Current File Number: ");
  Serial.println(dfPlayer.readCurrentFileNumber());
  Serial.print("File Counts In Folder 03: ");
  Serial.println(dfPlayer.readFileCountsInFolder(3)); // read file counts in folder SD:/03
  Serial.println("=================");
}

void printDetail(uint8_t type, int value)
{
  switch (type)
  {
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
    switch (value)
    {
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
} // end printDetail for DFPlayer

void dfPlayerUpdate(void)
{
  if (!isDFPlayerDetected) return;
  unsigned long timePlay = 3000; // Plays 3 seconds of all files.
  static unsigned long timer = millis();
  if (millis() - timer > timePlay)
  {
    //  if (millis() - timer > 10000) {
    timer = millis();
    dfPlayer.next(); // Play next mp3 every 3 second.
  }
  if (dfPlayer.available())
  {
    printDetail(dfPlayer.readType(), dfPlayer.read()); // Print the detail message from DFPlayer to handle different errors and states.
  }
} // end

// Functions to learn DFPlayer behavior
void playNotBusy()
{
  if (!isDFPlayerDetected) return;
  // Plays all files succsivly.
  Serial.println("PlayNotBusy");
  if (HIGH == digitalRead(nDFPlayer_BUSY))
  {
    // mp3_next ();
    dfPlayer.next();
  }
  if (dfPlayer.available())
  {
    printDetail(dfPlayer.readType(), dfPlayer.read()); // Print the detail message from DFPlayer to handle different errors and states.
  }
  delay(1000); // This should be removed from code. We set a time for the next allowed message.
  // Or better yet use interupt to find the end of BUSY and set time for the next allowed DFPlayer message
} // end playNotBusy

void playNotBusyLevel(int level)
{
  if (!isDFPlayerDetected) return;
  // Plays all files succsivly.
  Serial.println("PlayNotBusyLevel");
  if (HIGH == digitalRead(nDFPlayer_BUSY))
  {
    // mp3_next ();
    //  Note....we should in fact use file names, not numbers here,
    //  or must at least build a data structure to associate the two.
    //  that shoulde be future work.
    dfPlayer.play(level + 1);
    Serial.println("HIGH .next called! =================");
  }
  if (dfPlayer.available())
  {
    printDetail(dfPlayer.readType(), dfPlayer.read()); // Print the detail message from DFPlayer to handle different errors and states.
  }
  delay(1000); // This should be removed from code. We set a time for the next allowed message.
  // Or better yet use interupt to find the end of BUSY and set time for the next allowed DFPlayer message
}

// Play a track but not if the DFPlayer is busy
bool playAlarmLevel(int alarmNumberToPlay)
{
  if (!isDFPlayerDetected) return false;

  static unsigned long timer = 0;
  const unsigned long delayPlayLevel = 20;

  int trackNumber = alarmNumberToPlay;

  if (millis() - timer <= delayPlayLevel)
  {
    return false;
  }

  timer = millis();

  // Refresh file count if it was not read yet.
  if (numberFilesDF <= 0)
  {
    numberFilesDF = dfPlayer.readFileCounts();
  }

  if (trackNumber <= 0 || trackNumber > numberFilesDF)
  {
    Serial.print("Invalid DFPlayer track number: ");
    Serial.print(trackNumber);
    Serial.print(". Available files: ");
    Serial.println(numberFilesDF);
    return false;
  }

  if (HIGH == digitalRead(nDFPlayer_BUSY))
  {
    dfPlayer.play(trackNumber);
    Serial.print("Playing DFPlayer track: ");
    Serial.println(trackNumber);
  }
  else
  {
    Serial.println("DFPlayer is busy; previous file is still playing.");
    return false;
  }

  if (dfPlayer.available())
  {
    printDetail(dfPlayer.readType(), dfPlayer.read());
  }

  return true;
} // end of playAlarmLevel

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
