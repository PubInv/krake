

/*
 Wink the LED
 Similar to the Wink function for hardware.
 */

int HIGH_TIME_LED = 900;
int LOW_TIME_LED = 100;
long lastLEDtime = 0;
long nextLEDchange = 100; //time in ms.
boolean LED_BUILTIN = true;

color myLEDColor = color(255, 16, 16);  //Start red

void updateLED() {
  if (((millis() - lastLEDtime) > nextLEDchange) || (millis() < lastLEDtime)) {
    if ((LED_BUILTIN) == false) {
      //set LED to on
      myLEDColor = color(255, 16, 16);
      stroke(13);
      LED_BUILTIN = true;
      nextLEDchange = HIGH_TIME_LED;
    } else {
      //set LED to off
      myLEDColor = color(25, 0, 0);
      stroke(255);
      LED_BUILTIN = false;
      nextLEDchange = LOW_TIME_LED;
    }
    lastLEDtime = millis();
  }  
  fill(myLEDColor);
}//end update LED
