//
// Author: Forrest Lee Erickson
// Date: 20241013
// LICENSE "GNU Affero General Public License, version 3 "



// Heart beat aka activity indicator LED.
//Set LED for Uno or ESP32 Dev Kit on board blue LED.
#include "Arduino.h"


//Wink the LED
void wink(void) {
  // TODO
  const int LED_BUILTIN = 13;  //ESP32 Kit//const int LED_BUILTIN = 2; HWK2 // Not really needed for Arduino UNO it is defined in library
  pinMode(LED_BUILTIN, OUTPUT);
  // const int HIGH_TIME_LED = 900;
  // const int LOW_TIME_LED = 100;
  const int HIGH_TIME_LED = 1400;
  const int LOW_TIME_LED = 500;
  static unsigned long lastLEDtime = 0;
  static unsigned long nextLEDchange = 500;  //time in ms.
  if (((millis() - lastLEDtime) > nextLEDchange) || (millis() < lastLEDtime)) {
    if (digitalRead(LED_BUILTIN) == LOW) {
      digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
      nextLEDchange = HIGH_TIME_LED;
    } else {
      digitalWrite(LED_BUILTIN, LOW);  // turn the LED on (HIGH is the voltage level)
      nextLEDchange = LOW_TIME_LED;
    }
    lastLEDtime = millis();
  }
}  //end LED wink
