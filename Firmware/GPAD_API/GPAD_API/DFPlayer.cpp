#include "DFPlayer.h"
#include "gpad_utility.h"
#include <DFRobotDFPlayerMini.h>

DFRobotDFPlayerMini dfPlayer;
HardwareSerial mySerial1(2); // Use UART2

const int LED_PIN = 13; // Krake
const int nDFPlayer_BUSY = 4; // active LOW BUSY pin from DFPlayer

bool isDFPlayerDetected = false;
int volumeDFPlayer = 20; // Range: 0 to 30
int numberFilesDF = 0;   // Number of audio files found on SD card

char command;
int pausa = 0;

void serialSplashDFP()
{
  Serial.println("===================================");
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(PROG_NAME);
  Serial.println(FIRMWARE_VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__));
  Serial.println("===================================");
  Serial.println();
}

void menu_opcoes()
{
  Serial.println();
  Serial.println(F("=================================================================================================================================="));
  Serial.println(F("Commands:"));
  Serial.println(F(" [1-9] select MP3 file"));
  Serial.println(F(" [s] stop playback"));
  Serial.println(F(" [p] pause/continue"));
  Serial.println(F(" [+/-] increase/decrease volume"));
  Serial.println(F(" [</>] previous/next track"));
  Serial.println(F("================================================================================================================================="));
}

void checkSerial(void)
{
  while (Serial.available() > 0)
  {
    command = Serial.read();

    if ((command >= '1') && (command <= '9'))
    {
      int track = command - '0';
      Serial.print("Playing track: ");
      Serial.println(track);
      dfPlayer.play(track);
      menu_opcoes();
    }

    if (command == 's')
    {
      dfPlayer.stop();
      Serial.println("Music stopped.");
      menu_opcoes();
    }

    if (command == 'p')
    {
      pausa = !pausa;
      if (pausa == 0)
      {
        Serial.println("Continue...");
        dfPlayer.start();
      }
      else
      {
        Serial.println("Music paused.");
        dfPlayer.pause();
      }
      menu_opcoes();
    }

    if (command == '+')
    {
      dfPlayer.volumeUp();
      Serial.print("Current volume: ");
      Serial.println(dfPlayer.readVolume());
      menu_opcoes();
    }

    if (command == '-')
    {
      dfPlayer.volumeDown();
      Serial.print("Current volume: ");
      Serial.println(dfPlayer.readVolume());
      menu_opcoes();
    }

    if (command == '<')
    {
      dfPlayer.previous();
      Serial.println("Previous track.");
      menu_opcoes();
    }

    if (command == '>')
    {
      dfPlayer.next();
      Serial.println("Next track.");
      menu_opcoes();
    }
  }
}

void setupDFPlayer()
{
  pinMode(nDFPlayer_BUSY, INPUT_PULLUP);

  Serial.println("UART2 Begin for DFPlayer");
  mySerial1.begin(BAUD_DFPLAYER, SERIAL_8N1, RXD2, TXD2);
  delay(1000);

  // ACK=false is safer for DFPlayer clones and avoids repeated blocking/timeouts.
  Serial.println("Begin DFPlayer: ACK=false, doReset=false");
  if (!dfPlayer.begin(mySerial1, false, false))
  {
    Serial.println("DFPlayer Mini not detected or not responding.");
    Serial.println("Check wiring, power, SD card, and file names.");
    isDFPlayerDetected = false;
    return;
  }

  isDFPlayerDetected = true;
  Serial.println("DFPlayer Mini detected.");

  dfPlayer.setTimeOut(500);
  delay(300);

  // This may return unusual values on clones. Do not disable audio only because of this.
  int moduleState = dfPlayer.readState();
  Serial.print("DFPlayer state after init: ");
  Serial.println(moduleState);
  if (moduleState > 0)
  {
    Serial.println("Warning: unusual DFPlayer state. Possible clone/module variant, continuing test.");
  }

  dfPlayer.volume(volumeDFPlayer);
  delay(300);

  numberFilesDF = dfPlayer.readFileCounts();
  Serial.print("SD card file count: ");
  Serial.println(numberFilesDF);

  if (numberFilesDF <= 0)
  {
    Serial.println("Warning: no audio files detected. Use FAT32 SD card and files like 0001.mp3, 0002.mp3.");
  }

  Serial.println("DFPlayer startup test: playing track 1.");
  dfPlayer.play(1);
  delay(3000); // Give enough time to hear output. Do not immediately stop.

  displayDFPlayerStats();
  menu_opcoes();
}

void setVolume(int zeroToThirty)
{
  if (zeroToThirty < 0) zeroToThirty = 0;
  if (zeroToThirty > 30) zeroToThirty = 30;

  volumeDFPlayer = zeroToThirty;
  if (isDFPlayerDetected)
  {
    dfPlayer.volume(volumeDFPlayer);
  }
}

void displayDFPlayerStats()
{
  if (!isDFPlayerDetected)
  {
    Serial.println("DFPlayer stats unavailable: module not detected.");
    return;
  }

  Serial.println("================= DFPlayer Stats =================");
  Serial.print("DFPlayer State: ");
  Serial.println(dfPlayer.readState());

  Serial.print("DFPlayer Volume: ");
  Serial.println(dfPlayer.readVolume());

  Serial.print("DFPlayer EQ: ");
  Serial.println(dfPlayer.readEQ());

  Serial.print("SD Card File Count: ");
  numberFilesDF = dfPlayer.readFileCounts();
  Serial.println(numberFilesDF);

  Serial.print("Current File Number: ");
  Serial.println(dfPlayer.readCurrentFileNumber());

  Serial.print("BUSY pin: ");
  Serial.println(digitalRead(nDFPlayer_BUSY) == LOW ? "LOW / playing" : "HIGH / idle");
  Serial.println("==================================================");
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
    Serial.println(F("USB Inserted!"));
    break;
  case DFPlayerUSBRemoved:
    Serial.println(F("USB Removed!"));
    break;
  case DFPlayerPlayFinished:
    Serial.print(F("Track "));
    Serial.print(value);
    Serial.println(F(" finished."));
    break;
  case DFPlayerError:
    Serial.print(F("DFPlayerError: "));
    switch (value)
    {
    case Busy:
      Serial.println(F("Card not found or module busy"));
      break;
    case Sleeping:
      Serial.println(F("Sleeping"));
      break;
    case SerialWrongStack:
      Serial.println(F("Wrong serial stack"));
      break;
    case CheckSumNotMatch:
      Serial.println(F("Checksum mismatch"));
      break;
    case FileIndexOut:
      Serial.println(F("File index out of bound"));
      break;
    case FileMismatch:
      Serial.println(F("Cannot find file"));
      break;
    case Advertise:
      Serial.println(F("In advertise"));
      break;
    default:
      Serial.println(value);
      break;
    }
    break;
  default:
    break;
  }
}

void dfPlayerUpdate(void)
{
  if (!isDFPlayerDetected) return;

  if (dfPlayer.available())
  {
    printDetail(dfPlayer.readType(), dfPlayer.read());
  }
}

void playNotBusy()
{
  if (!isDFPlayerDetected) return;

  Serial.println("playNotBusy");
  if (digitalRead(nDFPlayer_BUSY) == HIGH)
  {
    dfPlayer.next();
  }
  else
  {
    Serial.println("DFPlayer is still busy/playing.");
  }

  if (dfPlayer.available())
  {
    printDetail(dfPlayer.readType(), dfPlayer.read());
  }
}

void playNotBusyLevel(int level)
{
  if (!isDFPlayerDetected) return;

  Serial.println("playNotBusyLevel");
  if (digitalRead(nDFPlayer_BUSY) == HIGH)
  {
    dfPlayer.play(level + 1);
    Serial.println("Track command sent.");
  }
  else
  {
    Serial.println("DFPlayer is still busy/playing.");
  }

  if (dfPlayer.available())
  {
    printDetail(dfPlayer.readType(), dfPlayer.read());
  }
}

bool playAlarmLevel(int alarmNumberToPlay)
{
  if (!isDFPlayerDetected) return false;

  static unsigned long timer = 0;
  const unsigned long delayPlayLevel = 100;

  if (millis() - timer <= delayPlayLevel)
  {
    return false;
  }
  timer = millis();

  if (numberFilesDF <= 0)
  {
    numberFilesDF = dfPlayer.readFileCounts();
  }

  int trackNumber = alarmNumberToPlay;

  if (trackNumber <= 0 || trackNumber > numberFilesDF)
  {
    Serial.print("Invalid DFPlayer track number: ");
    Serial.println(trackNumber);
    return false;
  }

  if (digitalRead(nDFPlayer_BUSY) == HIGH)
  {
    Serial.print("Playing alarm track: ");
    Serial.println(trackNumber);
    dfPlayer.play(trackNumber);
  }
  else
  {
    Serial.println("Not done playing previous file.");
    return false;
  }

  if (dfPlayer.available())
  {
    printDetail(dfPlayer.readType(), dfPlayer.read());
  }

  return true;
}
