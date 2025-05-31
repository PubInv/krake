#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>

using namespace Menu;

#define LEDPIN 12
#define MAX_DEPTH 1

unsigned int timeOn=10;
unsigned int timeOff=90;


result action1(eventMask e) {
  if (e == navCmds::enterCmd) {
      Serial.println(F("Yes, I will take that action #1 !"));
  }
    // I don't know what this does.
  return proceed;
}
result action2(eventMask e) {
  if (e == navCmds::selCmd) {
      Serial.println(F("Yes, I will take that action #2 !"));
  }
    // I don't know what this does.
  return proceed;
}
result action3(eventMask e) {
  if (e == navCmds::enterCmd) {
      Serial.println(F("Yes, I will take that action #3 !"));
  }
  // I don't know what this does.
  return proceed;
}

MENU(mainMenu, "Blink menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,activateEvent)
  ,OP("Op3",action2,anyEvent)
  ,EXIT("<Back")
);

serialIn serial(Serial);
MENU_INPUTS(in,&serial);

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

void setup_GPAD_menu() {
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Menu 4.x");
  Serial.println("Use keys + - * /");
  Serial.println("to control the menu navigation");
}

bool blink(int timeOn,int timeOff) {return millis()%(unsigned long)(timeOn+timeOff)<(unsigned long)timeOn;}

void poll_GPAD_menu() {
  nav.poll();
  digitalWrite(LEDPIN, blink(timeOn,timeOff));
}

void navigate_to_n_and_execute(int n) {
  Serial.println("moving to zero and executing!");
  nav.doNav(navCmd(idxCmd,n)); //hilite second option
  nav.doNav(navCmd(selCmd)); //execute option
}