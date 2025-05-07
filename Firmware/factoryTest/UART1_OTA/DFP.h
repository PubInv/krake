#ifndef DFP_H
#define DFP_H


#include <Arduino.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Declare variables but don't define them
extern HardwareSerial mySerial1;
// extern DFRobotDFPlayerMini dfPlayer;

#define BAUD_DFPLAYER 9600

extern const int trac1;
extern const int trac2;
extern bool availableDFPLAYER;
// extern HardwareSerial mySerial1;
// extern DFRobotDFPlayerMini dfPlayer;

void initDFP(); // declaration // gives a type 
void splashDFPlayer();

void printDetail(uint8_t type, int value);
void dfPlayerUpdate(void);

#endif  // DFPLAYER_H
