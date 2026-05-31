#ifndef DFPLAYER
#define DFPLAYER 1

#include <Arduino.h>

#define BAUD_DFPLAYER 9600 // UART2 baud rate to DFPlayer
#define TXD2 17            // ESP32 TX2 -> DFPlayer RX
#define RXD2 16            // ESP32 RX2 -> DFPlayer TX

void setupDFPlayer();
void displayDFPlayerStats();

bool playAlarmLevel(int alarmNumberToPlay);
void playNotBusy();
void playNotBusyLevel(int level);
void dfPlayerUpdate(void);

void printDetail(uint8_t type, int value);

void checkSerial(void);
void menu_opcoes();
void serialSplashDFP();

void setVolume(int oneToThirty);

extern int volumeDFPlayer;
extern int numberFilesDF;
extern bool isDFPlayerDetected;

#endif
