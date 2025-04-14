// DailyStruggleButton.cpp

#include "Button.h"
#include "DailyStruggleButton.h"


unsigned int longPressTime = 1000;
byte multiHitTarget = 2;
unsigned int multiHitTime = 400;

DailyStruggleButton myButton;
DailyStruggleButton SW1;
DailyStruggleButton SW2;
DailyStruggleButton SW3;
DailyStruggleButton SW4;

void splashserial() {
    Serial.println(F("==================================="));
    Serial.println(VERSION);
    Serial.println(MODEL_NAME);
    Serial.println(DEVICE_UNDER_TEST);
    Serial.print(F("Compiled at: "));
    Serial.println(F(__DATE__ " " __TIME__));
    Serial.println(LICENSE);
    Serial.println(F("==================================="));
    Serial.println();
}

void setupButton() {

    myButton.set(GPIO0, buttonEvent, INT_PULL_UP);
    SW1.set(GPIO_SW1, morisCodeEvent, INT_PULL_UP);
    SW2.set(GPIO_SW2, RepeatCalculation, INT_PULL_UP);
    SW3.set(GPIO_SW3, muteFiveMin, INT_PULL_UP);
    SW4.set(GPIO_SW4, SendEmergMessage, INT_PULL_UP);

    myButton.enableLongPress(longPressTime);
    // SW1.enableLongPress(longPressTime);  //Also rotary encoder on KRAKE
    // SW2.enableLongPress(longPressTime);  //Also rotary encoder on KRAKE
    // SW3.enableLongPress(longPressTime);  //Also rotary encoder switch on KRAKE
    SW4.enableLongPress(longPressTime);

    // SW1.enableMultiHit(multiHitTime, multiHitTarget);
    // SW2.enableMultiHit(multiHitTime, multiHitTarget);
    // SW3.enableMultiHit(multiHitTime, multiHitTarget);
    SW4.enableMultiHit(multiHitTime, multiHitTarget);
}

void loopButton() {
    myButton.poll();
    SW1.poll();
    SW2.poll();
    SW3.poll();
    SW4.poll();
}

void handleButtonEvent(const char* buttonName, byte btnStatus) {
    switch (btnStatus) {
        case onPress:
            Serial.printf("%s Button Pressed\n", buttonName);
            break;
        case onLongPress:
            Serial.printf("%s Button Long Pressed For %d ms\n", buttonName, longPressTime);
            break;
        case onMultiHit:
            Serial.printf("%s Button Pressed %d times in %d ms\n", buttonName, multiHitTarget, multiHitTime);
            break;
    }
}

void buttonEvent(byte btnStatus) { handleButtonEvent("BOOT", btnStatus); }
void morisCodeEvent(byte btnStatus) { handleButtonEvent("SW1", btnStatus); }
void RepeatCalculation(byte btnStatus) { handleButtonEvent("SW2", btnStatus); }
void muteFiveMin(byte btnStatus) { handleButtonEvent("SW3", btnStatus); }
void SendEmergMessage(byte btnStatus) { handleButtonEvent("SW4", btnStatus); }
