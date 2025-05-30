// DailyStruggleButton.cpp

#include "Button.h"
#include "DailyStruggleButton.h"

const char* messageToPublish = nullptr;
int syntheticBPM = 0;  // Use when there is no BPM hardware

unsigned int longPressTime = 1000;
byte multiHitTarget = 2;
unsigned int multiHitTime = 400;

DailyStruggleButton myButton;
DailyStruggleButton SW1;
DailyStruggleButton SW2;
DailyStruggleButton SW3;
DailyStruggleButton SW4;
DailyStruggleButton BOOT;



void setupButton() {

  //  myButton.set(GPIO0, buttonEvent, INT_PULL_UP);
  SW1.set(GPIO_SW1, morisCodeEvent, INT_PULL_UP);
  // SW2.set(GPIO_SW2, RepeatCalculation, INT_PULL_UP);
  SW3.set(GPIO_SW3, muteFiveMin, INT_PULL_UP);
  SW4.set(GPIO_SW4, SendEmergMessage, INT_PULL_UP);
  BOOT.set(GPIO0_BOOT, SendOK_Message, INT_PULL_UP);

  //FLE  myButton.enableLongPress(longPressTime);
  // SW1.enableLongPress(longPressTime);  //Also rotary encoder on KRAKE
  // SW2.enableLongPress(longPressTime);  //Also rotary encoder on KRAKE
  // SW3.enableLongPress(longPressTime);  //Also rotary encoder switch on KRAKE
  SW4.enableLongPress(longPressTime);

  // SW1.enableMultiHit(multiHitTime, multiHitTarget);
  // SW2.enableMultiHit(multiHitTime, multiHitTarget);
  // SW3.enableMultiHit(multiHitTime, multiHitTarget);
  SW4.enableMultiHit(multiHitTime, multiHitTarget);
}

void buttonEvent(byte btnStatus) {
  // Your implementation here
}

void loopButton() {
  //FLE myButton.poll();
  SW1.poll();
  //SW2.poll();
  SW3.poll();
  SW4.poll();
  BOOT.poll();  //Reset WiFi Credentials
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

// void buttonEvent(byte btnStatus) {
//   handleButtonEvent("BOOT", btnStatus);
// }
// void morisCodeEvent(byte btnStatus) {
//   handleButtonEvent("SW1", btnStatus);
// }
// void RepeatCalculation(byte btnStatus) {
//   handleButtonEvent("SW2", btnStatus);
// }
// void muteFiveMin(byte btnStatus) {
//   handleButtonEvent("SW3", btnStatus);
// }
// void SendEmergMessage(byte btnStatus) {
//   handleButtonEvent("SW4", btnStatus);
// }


// void RepeatCalculation(byte btnStatus) {
//   handleButtonEvent("BOOT", btnStatus);
//   messageToPublish = "a1Booting PMD";
//   syntheticBPM = 85; //An OK BPM
// }

void morisCodeEvent(byte btnStatus) {
  handleButtonEvent("SW1", btnStatus);
  messageToPublish = "a2Check in please.";
  syntheticBPM = 55;  //A low  BPM
}

// void RepeatCalculation(byte btnStatus) {
//   handleButtonEvent("SW2", btnStatus);
//   messageToPublish = "Message from SW2";
// }

void muteFiveMin(byte btnStatus) {
  handleButtonEvent("SW3", btnStatus);
  messageToPublish = "a4Urgent Support Now.";
  syntheticBPM = 85;  //An g OK BPM
}

void SendEmergMessage(byte btnStatus) {
  handleButtonEvent("SW4", btnStatus);
  messageToPublish = "a5Send Emergency";
  syntheticBPM = 125;  //A high BPM
}


void SendOK_Message(byte btnStatus) {
  if (btnStatus == onPress) {
    //  handleButtonEvent("BOOT", btnStatus);
    //foo  FLE
    messageToPublish = "a1 Clearing WiFi Credentials, Situation Normal.";
    //ElegantOTA.clearAuth();
    clearOTA = true;
    Serial.println("Clearing web credentials now.");

    syntheticBPM = 75;  //An OK BPM
  }
}
