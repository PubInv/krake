#include "DFPlayer.h"
#include "gpad_utility.h"
#include "debug_macros.h"
#include <DFRobotDFPlayerMini.h>

DFRobotDFPlayerMini dfPlayer;
extern HardwareSerial uartSerial2;

const int LED_PIN = 13; // Krake
const int nDFPlayer_BUSY = 4; // active LOW BUSY pin from DFPlayer

bool isDFPlayerDetected = false;
int volumeDFPlayer = 20; // Range: 1 to 30
int numberFilesDF = 0;   // Number of audio files found on SD card
extern bool currentlyMuted;
char command;
int pausa = 0;

void serialSplashDFP()
{
  DBG_PRINTLN(F("==================================="));
  DBG_PRINTLN(F(DEVICE_UNDER_TEST));
  DBG_PRINT(F(PROG_NAME));
  DBG_PRINTLN(F(FIRMWARE_VERSION));
  DBG_PRINT(F("Compiled at: "));
  DBG_PRINTLN(F(__DATE__ " " __TIME__));
  DBG_PRINTLN(F("==================================="));
}

void menu_opcoes()
{
  DBG_PRINTLN(F(""));
  DBG_PRINTLN(F("=================================================================================================================================="));
  DBG_PRINTLN(F("Commands:"));
  DBG_PRINTLN(F(" [1-9] select MP3 file"));
  DBG_PRINTLN(F(" [s] stop playback"));
  DBG_PRINTLN(F(" [p] pause/continue"));
  DBG_PRINTLN(F(" [+/-] increase/decrease volume"));
  DBG_PRINTLN(F(" [</>] previous/next track"));
  DBG_PRINTLN(F("================================================================================================================================="));
}

void checkSerial(void)
{
  while (Serial.available() > 0)
  {
    command = Serial.read();

    if ((command >= '1') && (command <= '9'))
    {
      int track = command - '0';
      DBG_PRINT(F("Playing track: "));
      DBG_PRINTLN(track);
      dfPlayer.play(track);
      menu_opcoes();
    }

    if (command == 's')
    {
      dfPlayer.stop();
      DBG_PRINTLN(F("Music stopped."));
      menu_opcoes();
    }

    if (command == 'p')
    {
      pausa = !pausa;
      if (pausa == 0)
      {
        DBG_PRINTLN(F("Continue..."));
        dfPlayer.start();
      }
      else
      {
        DBG_PRINTLN(F("Music paused."));
        dfPlayer.pause();
      }
      menu_opcoes();
    }

    if (command == '+')
    {
      dfPlayer.volumeUp();
      DBG_PRINT(F("Current volume: "));
      DBG_PRINTLN(dfPlayer.readVolume());
      menu_opcoes();
    }

    if (command == '-')
    {
      dfPlayer.volumeDown();
      DBG_PRINT(F("Current volume: "));
      DBG_PRINTLN(dfPlayer.readVolume());
      menu_opcoes();
    }

    if (command == '<')
    {
      dfPlayer.previous();
      DBG_PRINTLN(F("Previous track."));
      menu_opcoes();
    }

    if (command == '>')
    {
      dfPlayer.next();
      DBG_PRINTLN(F("Next track."));
      menu_opcoes();
    }
  }
}

namespace
{
void delayWithYield(const unsigned long durationMs)
{
  const unsigned long startMs = millis();
  while ((millis() - startMs) < durationMs)
  {
    delay(10);
    yield();
  }
}
}

void setupDFPlayer()
{
  pinMode(nDFPlayer_BUSY, INPUT_PULLUP);

  DBG_PRINTLN(F("UART2 Begin for DFPlayer"));
  uartSerial2.begin(BAUD_DFPLAYER, SERIAL_8N1, RXD2, TXD2);
  delayWithYield(1000);

  // ACK=false is safer for DFPlayer clones and avoids repeated blocking/timeouts.
  DBG_PRINTLN(F("Begin DFPlayer: ACK=false, doReset=false"));
  if (!dfPlayer.begin(uartSerial2, false, false))
  {
    DBG_PRINTLN(F("DFPlayer Mini not detected or not responding."));
    DBG_PRINTLN(F("Check wiring, power, SD card, and file names."));
    isDFPlayerDetected = false;
    return;
  }

  isDFPlayerDetected = true;
  DBG_PRINTLN(F("DFPlayer Mini detected."));

  dfPlayer.setTimeOut(500);
  delayWithYield(300);

  // This may return unusual values on clones. Do not disable audio only because of this.
  int moduleState = dfPlayer.readState();
  DBG_PRINT(F("DFPlayer state after init: "));
  DBG_PRINTLN(moduleState);
  if (moduleState > 0)
  {
    DBG_PRINTLN(F("Warning: unusual DFPlayer state. Possible clone/module variant, continuing test."));
  }

  dfPlayer.volume(volumeDFPlayer);
  delayWithYield(300);

  numberFilesDF = dfPlayer.readFileCounts();
  DBG_PRINT(F("SD card file count: "));
  DBG_PRINTLN(numberFilesDF);

  if (numberFilesDF <= 0)
  {
    DBG_PRINTLN(F("Warning: no audio files detected. Use FAT32 SD card and files like 0001.mp3, 0002.mp3."));
  }

  DBG_PRINTLN(F("DFPlayer startup test: playing track 1."));
  dfPlayer.play(1);
  delayWithYield(3000); // Give enough time to hear output without starving the scheduler/WDT.

  displayDFPlayerStats();
  menu_opcoes();
}

void setVolume(int oneToThirty)
{
  if (oneToThirty < 1) oneToThirty = 1;
  if (oneToThirty > 30) oneToThirty = 30;

  volumeDFPlayer = oneToThirty;  
  if (isDFPlayerDetected)
  {
    dfPlayer.volume(volumeDFPlayer);
  }
}

void displayDFPlayerStats()
{
  if (!isDFPlayerDetected)
  {
    DBG_PRINTLN(F("DFPlayer stats unavailable: module not detected."));
    return;
  }

  DBG_PRINTLN(F("================= DFPlayer Stats ================="));
  DBG_PRINT(F("DFPlayer State: "));
  DBG_PRINTLN(dfPlayer.readState());

  DBG_PRINT(F("DFPlayer Volume: "));
  DBG_PRINTLN(dfPlayer.readVolume());

  DBG_PRINT(F("DFPlayer EQ: "));
  DBG_PRINTLN(dfPlayer.readEQ());

  DBG_PRINT(F("SD Card File Count: "));
  numberFilesDF = dfPlayer.readFileCounts();
  DBG_PRINTLN(numberFilesDF);

  DBG_PRINT(F("Current File Number: "));
  DBG_PRINTLN(dfPlayer.readCurrentFileNumber());

  DBG_PRINT(F("BUSY pin: "));
  DBG_PRINTLN(digitalRead(nDFPlayer_BUSY) == LOW ? F("LOW / playing") : F("HIGH / idle"));
  DBG_PRINTLN(F("=================================================="));
}

void printDetail(uint8_t type, int value)
{
#if (DEBUG_LEVEL > 0)
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
#else
  (void)type;
  (void)value;
#endif
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

  DBG_PRINTLN(F("playNotBusy"));
  if (digitalRead(nDFPlayer_BUSY) == HIGH)
  {
    dfPlayer.next();
  }
  else
  {
    DBG_PRINTLN(F("DFPlayer is still busy/playing."));
  }

  if (dfPlayer.available())
  {
    printDetail(dfPlayer.readType(), dfPlayer.read());
  }
}

void playNotBusyLevel(int level)
{
  if (!isDFPlayerDetected) return;

  if (currentlyMuted)
  {
    DBG_PRINTLN(F("Muted: skipping DFPlayer playback."));
    return;
  }

     if (level <= 0)
  {
    DBG_PRINTLN(F("Silent level: skipping DFPlayer playback."));
    return;
  }

  DBG_PRINTLN(F("playNotBusyLevel"));
  if (digitalRead(nDFPlayer_BUSY) == HIGH)
  {
    dfPlayer.play(level + 1);
    DBG_PRINTLN(F("Track command sent."));
  }
  else
  {
    DBG_PRINTLN(F("DFPlayer is still busy/playing."));
  }

  if (dfPlayer.available())
  {
    printDetail(dfPlayer.readType(), dfPlayer.read());
  }

  if (!isDFPlayerDetected) return;

}

bool playAlarmLevel(int alarmNumberToPlay)
{
  if (!isDFPlayerDetected) return false;

  if (currentlyMuted)
  {
    DBG_PRINTLN(F("Muted: skipping alarm playback."));
    return false;
  }

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
    DBG_PRINT(F("Invalid DFPlayer track number: "));
    DBG_PRINTLN(trackNumber);
    return false;
  }

  if (digitalRead(nDFPlayer_BUSY) == HIGH)
  {
    DBG_PRINT(F("Playing alarm track: "));
    DBG_PRINTLN(trackNumber);
    dfPlayer.play(trackNumber);
  }
  else
  {
    DBG_PRINTLN(F("Not done playing previous file."));
    return false;
  }

  if (dfPlayer.available())
  {
    printDetail(dfPlayer.readType(), dfPlayer.read());
  }

  return true;
}
