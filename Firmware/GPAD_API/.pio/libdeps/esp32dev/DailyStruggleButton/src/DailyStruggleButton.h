/*
DailyStruggleButton by cygig

DailyStruggleButton is yet another Arduino library to debounce button (push tactile switch) and manage its events. 
Events supported include pressing, releasing, holding down, long pressing (hold button for x time) and 
multi-hitting (hit button x times in y  time). For simplicity, there is only one callback function for all events, 
that passes an identifier as a parameter to indicate the event that just happened.
*/

#ifndef DAILYSTRUGGLE_H
#define DAILYSTRUGGLE_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define INT_PULL_UP 0
#define EXT_PULL_UP 1
#define EXT_PULL_DOWN 2

/* Pressed and released are purposely inverted 
as this is the most common use case, 
using internal pullup on pinMode().*/
#define RELEASED 1
#define PRESSED 0

#define neverPressed 0
#define onPress 1
#define onRelease 2
#define onHold 3
#define onLongPress 4
#define onMultiHit 5

class DailyStruggleButton{
  
  public:
    DailyStruggleButton();
    void set( byte myPin, void (*myCallback)(byte buttonEvent) );
    void set( byte myPin, void (*myCallback)(byte buttonEvent), byte myPull );
    bool poll();
    void setDebounceTime(unsigned int myTime);
    
    void enableLongPress(unsigned int myTime);
    void disableLongPress();
    
    void enableMultiHit(unsigned int myTime, byte myTarget);
    void disableMultiHit();
    

  private: 
    byte pin;
    byte buttonEvent=neverPressed;
    byte buttonPull=INT_PULL_UP;
      
    unsigned int debounceTime=20;
    unsigned long lastChangeTime=0;
    
    unsigned int longPressTime;
    unsigned long longPressSince;
    
    unsigned int multiHitTime;
    unsigned long multiHitSince;
    byte multiHitTarget;
    byte multiHitCount=0;

    void (*callback)(byte buttonEvent);
    
    struct {
      bool inverted:1;
      bool currentRawState:1;
      bool lastRawState:1;
      bool currentState:1;
      bool lastState:1;
      bool enableLongPress:1;
      bool wasLongPressed:1;
      bool enableMultiHit:1;
    } flags;

    bool debounce();
};

#endif
