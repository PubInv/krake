#ifndef DFPLAYER
#define DFPLAYER 1
#include <Arduino.h>

#define BAUD_DFPLAYER 9600  //for UART2 to the DFPlayer
#define TXD2 17
#define RXD2 16



void displayDFPlayerStats();

void setupDFPlayer();

bool playAlarmLevel(int alarmNumberToPlay);
void playNotBusy();
void dfPlayerUpdate(void);

void printDetail(uint8_t type, int value);

void displayDFPlayerStats();

void checkSerial(void);
void menu_opcoes();
void serialSplashDFP();

// void setupDFP();

#endif