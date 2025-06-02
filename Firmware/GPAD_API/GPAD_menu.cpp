#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>
#include <menuIO/rotaryEventIn.h>
#include "GPAD_hal.h"
#include "RickmanLiquidCrystal_I2C.h"

using namespace Menu;

#define LEDPIN 12
#define MAX_DEPTH 1


result action1(eventMask e) {
  if (e == eventMask::enterEvent) {
      Serial.println(F("Yes, I will take that action #1 !"));
  }
    // I don't know what this does.
  return proceed;
}
result action2(eventMask e) {
  if (e == eventMask::enterEvent) {
      Serial.println(F("Yes, I will take that action #2 !"));
  }
    // I don't know what this does.
  return proceed;
}
result action3(eventMask e) {
  if (e == eventMask::enterEvent) {
      Serial.println(F("Yes, I will take that action #3 !"));
  }
  // I don't know what this does.
  return proceed;
}

MENU(mainMenu, "Blink menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,anyEvent)
  ,OP("Op3",action3,anyEvent)
  ,EXIT("<Back")
);




RotaryEventIn reIn(
  RotaryEventIn::EventType::BUTTON_CLICKED | // select
  RotaryEventIn::EventType::BUTTON_DOUBLE_CLICKED | // back
  RotaryEventIn::EventType::BUTTON_LONG_PRESSED | // also back
  RotaryEventIn::EventType::ROTARY_CCW | // up
  RotaryEventIn::EventType::ROTARY_CW // down
); // register capabilities, see AndroidMenu MenuIO/RotaryEventIn.h file
MENU_INPUTS(in,&reIn);

// serialIn serial(Serial);
// MENU_INPUTS(in,&serial);


MENU_OUTPUTS(out,MAX_DEPTH
//  ,SERIAL_OUT(Serial)
  ,LCD_OUT(lcd,{0,0,20,4})
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);


void registerRotationEvent(bool CW) {
  Serial.print("CW: ");
  Serial.println(CW);
  reIn.registerEvent(CW ? RotaryEventIn::EventType::ROTARY_CW 
                        : RotaryEventIn::EventType::ROTARY_CCW); 
}

void registerRotaryEncoderPress() {
  reIn.registerEvent(RotaryEventIn::EventType::BUTTON_CLICKED);
}


void setup_GPAD_menu() {

}


void poll_GPAD_menu() {
  nav.poll();
}

void navigate_to_n_and_execute(int n) {
  Serial.println("moving to zero and executing!");
  nav.doNav(navCmd(idxCmd,n)); //hilite second option
  //nav.doNav(navCmd(enterCmd)); //execute option
}