#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>
#include <menuIO/rotaryEventIn.h>
#include "GPAD_HAL.h"
#include "RickmanLiquidCrystal_I2C.h"
#include "DFPlayer.h"
#include "alarm_api.h"
#include "mqtt_handler.h"

using namespace Menu;


extern PubSubClient client;

extern bool running_menu;
extern bool menu_just_exited;
extern unsigned long muteTimeoutEndMillis;

#define LEDPIN 12
#define MAX_DEPTH 2

result action1(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    Serial.println(F("Yes, I will take that action #1 !"));
  }
  char onLineMsg[32] = "Acknowledging!";
  publishAck(&client, onLineMsg);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Acknowledged!");
  lcd.setCursor(0, 1);
  lcd.print("Alarm still active");
  return proceed;
}
result action2(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    Serial.println(F("Yes, I will take that action #2 !"));
  }
  char emptyMsg[] = "";
  alarm(silent, emptyMsg, &Serial);      // sets currentLevel=0, clears AlarmMessageBuffer
  annunciateAlarmLevel(&Serial);          // turns off LEDs, updates LCD, stops DFPlayer buzzer
  char onLineMsg[32] = "Dismissed!";
  publishAck(&client, onLineMsg);
  return proceed;
}
result action3(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    Serial.println(F("Yes, I will take that action #3 !"));
  }
  char emptyMsg[] = "";
  alarm(silent, emptyMsg, &Serial);
  annunciateAlarmLevel(&Serial);
  char onLineMsg[32] = "Shelved!";
  publishAck(&client, onLineMsg);
  return proceed;
}
result action4(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    Serial.println(F("Yes, I will take that action #3 !"));
  }
  Serial.print(F("volume value: "));
  Serial.println(volumeDFPlayer);
  setVolume(volumeDFPlayer);
  return proceed;
}
result action5(eventMask e)
{
  Serial.println("exiting menu");
  running_menu = false;
  menu_just_exited = true;
  Menu::doExit();
  return proceed;
}

result actionResetConfirm(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    Serial.println(F("Reset confirmed. Restarting device..."));
    delay(100);
    ESP.restart();
  }
  return proceed;
}

int muteTimeoutMinutes = 5;


const uint32_t kBaudOptions[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
const uint8_t kBaudOptionCount = sizeof(kBaudOptions) / sizeof(kBaudOptions[0]);
const char *kSerialFormats[] = {"8-N-1"};
const uint8_t kSerialFormatCount = sizeof(kSerialFormats) / sizeof(kSerialFormats[0]);
const char *kFlowControlModes[] = {"Off", "RTS-CTS"};

int comBaudRate = 9600;
int comSerialFormatIndex = 0;
int comFlowControlIndex = 0;



result actionComSaveAndExit(eventMask e)
{
  if (e != eventMask::enterEvent)
  {
    return proceed;
  }

  bool baudOk = false;
  for (uint8_t i = 0; i < kBaudOptionCount; i++)
  {
    if (kBaudOptions[i] == static_cast<uint32_t>(comBaudRate))
    {
      baudOk = true;
      break;
    }
  }
  if (!baudOk)
  {
    comBaudRate = 9600;
  }

  if (comSerialFormatIndex < 0 || comSerialFormatIndex >= kSerialFormatCount)
  {
    comSerialFormatIndex = 0;
  }
  if (comFlowControlIndex < 0 || comFlowControlIndex > 1)
  {
    comFlowControlIndex = 0;
  }

  setComPortBaudRate(static_cast<uint32_t>(comBaudRate));
  setComPortSerialFormatIndex(static_cast<uint8_t>(comSerialFormatIndex));
  setComPortFlowControl(comFlowControlIndex == 1 ? COM_FLOW_RTS_CTS : COM_FLOW_OFF);
  applyComPortConfig(&Serial);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("COM saved");
  lcd.setCursor(0, 1);
  lcd.print(comBaudRate);
  lcd.print(" ");
  lcd.print(kSerialFormats[comSerialFormatIndex]);
  lcd.setCursor(0, 2);
  lcd.print("Flow:");
  lcd.print(kFlowControlModes[comFlowControlIndex]);
  return quit;
}

result actionComExitNoSave(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    const ComPortConfig &cfg = getComPortConfig();
    comBaudRate = static_cast<int>(cfg.baudRate);
    comSerialFormatIndex = static_cast<int>(cfg.serialFormatIndex);
    comFlowControlIndex = (cfg.flowControl == COM_FLOW_RTS_CTS) ? 1 : 0;
    return quit;
  }
  return proceed;
}

result actionMuteTimeout(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    Serial.print(F("Mute timeout set: "));
    Serial.print(muteTimeoutMinutes);
    Serial.println(F(" min"));
    setMuteTimeoutMinutes((unsigned long)muteTimeoutMinutes);
    annunciateAlarmLevel(&Serial);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Muted for:");
    lcd.setCursor(0, 1);
    lcd.print(muteTimeoutMinutes);
    lcd.print(" minute(s)");
  }
  return proceed;
}


MENU(comSetupMenu, "COM Setup", Menu::doNothing, Menu::noEvent, Menu::wrapStyle,
  FIELD(comBaudRate, "Baud Rate", "", 1200, 115200, 9600, 1, Menu::doNothing, anyEvent, noStyle),
  FIELD(comSerialFormatIndex, "Serial Format", "", 0, 0, 0, 1, Menu::doNothing, anyEvent, noStyle),
  FIELD(comFlowControlIndex, "Flow Control", "", 0, 1, 0, 1, Menu::doNothing, anyEvent, noStyle),
  OP("Save & Exit", actionComSaveAndExit, enterEvent),
  OP("Exit (No Save)", actionComExitNoSave, enterEvent)
);

MENU(resetConfirmMenu, "Reset", Menu::doNothing, Menu::noEvent, Menu::noStyle,
  OP("Yes - Reset", actionResetConfirm, enterEvent),
  OP("No  - Cancel", Menu::doNothing, Menu::noEvent)
);


MENU(mainMenu, "Krake Menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle,
  OP("Acknowledge", action1, enterEvent),
  OP("Dismiss", action2, enterEvent),
  OP("Shelve", action3, enterEvent),
  FIELD(volumeDFPlayer, "Volume", "%", 0, 30, 10, 1, action4, anyEvent, wrapStyle),
  FIELD(muteTimeoutMinutes, "Mute Time", "min", 1, 60, 5, 1, actionMuteTimeout, enterEvent, wrapStyle),
  SUBMENU(comSetupMenu),
  SUBMENU(resetConfirmMenu),
  OP("Exit Menu", action5, enterEvent)
);

RotaryEventIn reIn(
    RotaryEventIn::EventType::BUTTON_CLICKED |        // select
    RotaryEventIn::EventType::BUTTON_DOUBLE_CLICKED | // back
    RotaryEventIn::EventType::BUTTON_LONG_PRESSED |   // also back
    RotaryEventIn::EventType::ROTARY_CCW |            // up
    RotaryEventIn::EventType::ROTARY_CW               // down
);                                                    // register capabilities, see AndroidMenu MenuIO/RotaryEventIn.h file
MENU_INPUTS(in, &reIn);

// serialIn serial(Serial);
// MENU_INPUTS(in,&serial);

MENU_OUTPUTS(out, MAX_DEPTH
             //  ,SERIAL_OUT(Serial)
             ,
             LCD_OUT(lcd, {0, 0, 20, 4}), NONE // must have 2 items at least
);

NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);

void registerRotationEvent(bool CW)
{
  Serial.print("CW: ");
  Serial.println(CW);
  // Note: Rob believes it is more "natural" for clockwise to mean "up".
  // Apparently, whoever wrote the "MENU_INPUTS" believes the opposite,
  // so I am changing this hear to reverse the sense.
  reIn.registerEvent(CW ? RotaryEventIn::EventType::ROTARY_CCW
                        : RotaryEventIn::EventType::ROTARY_CW);
}

void registerRotaryEncoderPress()
{
  reIn.registerEvent(RotaryEventIn::EventType::BUTTON_CLICKED);
}

void setup_GPAD_menu()
{
  const ComPortConfig &cfg = getComPortConfig();
  comBaudRate = static_cast<int>(cfg.baudRate);
  comSerialFormatIndex = static_cast<int>(cfg.serialFormatIndex);
  comFlowControlIndex = (cfg.flowControl == COM_FLOW_RTS_CTS) ? 1 : 0;
}

void poll_GPAD_menu()
{
  nav.poll();
}

void navigate_to_n_and_execute(int n)
{
  Serial.println("moving to zero and executing!");
  nav.doNav(navCmd(idxCmd, n)); // hilite second option
  // nav.doNav(navCmd(enterCmd)); //execute option
}

void reset_menu_navigation()
{
  running_menu = true;
  nav.reset();
}
