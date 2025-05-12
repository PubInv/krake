// DailyStruggleButton.h
#ifndef DAILY_STRUGGLE_BUTTON_H
#define DAILY_STRUGGLE_BUTTON_H

#include <Arduino.h>
#include <DailyStruggleButton.h>

#define GPIO_SW1 36
//#define GPIO_SW2 39
#define GPIO_SW3 34
#define GPIO_SW4 35
#define GPIO0 0

extern const char* messageToPublish ;
void buttonEvent(byte btnStatus);
extern unsigned int longPressTime;
extern byte multiHitTarget;
extern unsigned int multiHitTime;

extern DailyStruggleButton myButton;
extern DailyStruggleButton SW1;
//extern DailyStruggleButton SW2;
extern DailyStruggleButton SW3;
extern DailyStruggleButton SW4;

void splashserial();
void buttonEvent(byte btnStatus);
void morisCodeEvent(byte btnStatus);
//void RepeatCalculation(byte btnStatus);
void muteFiveMin(byte btnStatus);
void SendEmergMessage(byte btnStatus);
void setupButton() ;
void loopButton();
#endif
