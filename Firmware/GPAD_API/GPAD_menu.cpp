#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>
#include <menuIO/rotaryEventIn.h>
#include "GPAD_hal.h"
#include "RickmanLiquidCrystal_I2C.h"
#include "DFPlayer.h"

using namespace Menu;

#define LEDPIN 12
#define MAX_DEPTH 1

// This needs to be cleaned up; the PubSublcient and WiFi stuff needs to put into a 
// a module that is not in GPAD_API.ino
extern PubSubClient client;
extern char publish_Ack_Topic[17];

result action1(eventMask e) {
  if (e == eventMask::enterEvent) {
      Serial.println(F("Yes, I will take that action #1 !"));
  }
  char onLineMsg[32] = "Acknowledging!";
  client.publish(publish_Ack_Topic, onLineMsg);
  Serial.print("Messgage sent to topic: ");
  Serial.println(publish_Ack_Topic);
  return proceed;
}
result action2(eventMask e) {
  if (e == eventMask::enterEvent) {
      Serial.println(F("Yes, I will take that action #2 !"));
  }
  return proceed;
}
result action3(eventMask e) {
  if (e == eventMask::enterEvent) {
      Serial.println(F("Yes, I will take that action #3 !"));
  }
  return proceed;
}
result action4(eventMask e) {
  if (e == eventMask::enterEvent) {
      Serial.println(F("Yes, I will take that action #3 !"));
  }
  Serial.print(F("volume value: "));
  Serial.println(volumeDFPlayer);
  setVolume(volumeDFPlayer);
  return proceed;
}


MENU(mainMenu, "Blink menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Acknowledge",action1,anyEvent)
  ,OP("Dismiss",action2,anyEvent)
  ,OP("Shelve",action3,anyEvent)
  ,FIELD(volumeDFPlayer,"Volume","%",0,30,10,1,action4,anyEvent,wrapStyle)
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
  // Note: Rob believes it is more "natural" for clockwise to mean "up".
  // Apparently, whoever wrote the "MENU_INPUTS" believes the opposite,
  // so I am changing this hear to reverse the sense.
  reIn.registerEvent(CW ? RotaryEventIn::EventType::ROTARY_CCW 
                        : RotaryEventIn::EventType::ROTARY_CW); 
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