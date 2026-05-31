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
#include "debug_macros.h"

using namespace Menu;


extern PubSubClient client;
extern char currentAlarmId[11];
extern bool running_menu;
extern bool menu_just_exited;
extern unsigned long muteTimeoutEndMillis;
extern bool selectMqttBrokerOption(uint8_t index);

#define LEDPIN 12
#define MAX_DEPTH 3

static bool settingsExitRequested = false;

void reset_menu_navigation();
static void finishReturnToMainPage();

void returnToMainPage()
{
  if (running_menu)
  {
    settingsExitRequested = true;
    requestAlarmRefresh(&Serial, false);
    return;
  }

  finishReturnToMainPage();
}

result action1(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    DBG_PRINTLN(F("Acknowledging alarm"));
  }
  publishGPAPResponse(&client, "a", currentAlarmId);
  DBG_PRINT(F("GPAP response queued for ID: "));
  DBG_PRINTLN(currentAlarmId);
  requestAlarmRefresh(&Serial, false);
  return proceed;
}
result action2(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    DBG_PRINTLN(F("Dismissing alarm"));
  }
  char emptyMsg[] = "";
  alarm(silent, emptyMsg, &Serial);      // sets currentLevel=0, clears AlarmMessageBuffer
  requestAlarmRefresh(&Serial);           // coalesces LCD/audio updates from loop()
  publishGPAPResponse(&client, "d", currentAlarmId);
  DBG_PRINT(F("GPAP response queued for ID: "));
  DBG_PRINTLN(currentAlarmId);
  return proceed;
}
result action3(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    DBG_PRINTLN(F("Shelving alarm"));
  }
  char emptyMsg[] = "";
  alarm(silent, emptyMsg, &Serial);
  requestAlarmRefresh(&Serial);
  publishGPAPResponse(&client, "s", currentAlarmId);
  DBG_PRINT(F("GPAP response queued for ID: "));
  DBG_PRINTLN(currentAlarmId);
  return proceed;
}
result action4(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    DBG_PRINTLN(F("Saving volume"));
  }
  DBG_PRINT(F("volume value: "));
  DBG_PRINTLN(volumeDFPlayer);
  setVolume(volumeDFPlayer);
  return proceed;
}
// result action5(eventMask e)
// {
//   if (e & eventMask::enterEvent)
//   {
//     DBG_PRINTLN(F("exiting menu"));
//     returnToMainPage();
//     return proceed;
//   }
//   return proceed;
// }
result action5(eventMask e)
{
  if (e & eventMask::enterEvent)
  {
    DBG_PRINTLN(F("Completing alarm"));
    char emptyMsg[] = "";
    alarm(silent, emptyMsg, &Serial);
    requestAlarmRefresh(&Serial);
    publishGPAPResponse(&client, "c", currentAlarmId);
    DBG_PRINT(F("GPAP response queued for ID: "));
    DBG_PRINTLN(currentAlarmId);
  }
  return proceed;
}

result actionBack(eventMask e)
{
  if (e & eventMask::enterEvent)
  {
    lcd.noBlink();
    lcd.noCursor();
    requestAlarmRefresh(&Serial, false);
    return quit;
  }
  return proceed;
}

result actionSettingsBack(eventMask e)
{
  if (e & eventMask::enterEvent)
  {
    returnToMainPage();
  }
  return proceed;
}

result actionResetConfirm(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    DBG_PRINTLN(F("Reset confirmed. Restarting device..."));
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

  return quit;
}

result actionComExitNoSave(eventMask e)
{
  if (e & eventMask::enterEvent)
  {
    const ComPortConfig &cfg = getComPortConfig();
    comBaudRate = static_cast<int>(cfg.baudRate);
    comSerialFormatIndex = static_cast<int>(cfg.serialFormatIndex);
    comFlowControlIndex = (cfg.flowControl == COM_FLOW_RTS_CTS) ? 1 : 0;
    lcd.noBlink();
    lcd.noCursor();
    requestAlarmRefresh(&Serial, false);
    return quit;
  }
  return proceed;
}

result actionMuteTimeout(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    DBG_PRINT(F("Mute timeout set: "));
    DBG_PRINT(muteTimeoutMinutes);
    DBG_PRINTLN(F(" min"));
    requestAlarmRefresh(&Serial);
  }
  return proceed;
}

result actionMuteNow(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    setMuteTimeoutMinutes((unsigned long)muteTimeoutMinutes);
    requestAlarmRefresh(&Serial);
  }
  return proceed;
}

result actionUnmuteNow(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    clearMuteTimeout();
    setMuted(false);
    requestAlarmRefresh(&Serial);
  }
  return proceed;
}

result actionWifiStatus(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    showWifiStatusPage();
    running_menu = false;
    Menu::doExit();
  }
  return proceed;
}

bool selectBroker(uint8_t index)
{
  const bool selected = selectMqttBrokerOption(index);
  running_menu = false;
  menu_just_exited = false;
  Menu::doExit();
  resetLcdUiToMainPage();
  showActionFeedback(selected ? "Broker selected" : "Broker failed");
  requestAlarmRefresh(&Serial, false);
  return selected;
}

result actionBrokerPublic(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    selectBroker(0);
  }
  return proceed;
}

result actionBrokerKrake(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    selectBroker(1);
  }
  return proceed;
}

result actionInfo(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    running_menu = false;
    menu_just_exited = false;
    Menu::doExit();
    showInfoPage();
  }
  return proceed;
}

result actionDiagnostics(eventMask e)
{
  if (e == eventMask::enterEvent)
  {
    running_menu = false;
    menu_just_exited = false;
    Menu::doExit();
    showInfoPage();
  }
  return proceed;
}


MENU(comSetupMenu, "COM Setup", Menu::doNothing, Menu::noEvent, Menu::wrapStyle,
  FIELD(comBaudRate, "Baud Rate", "", 1200, 115200, 9600, 1, Menu::doNothing, anyEvent, noStyle),
  FIELD(comSerialFormatIndex, "Serial Format", "", 0, 0, 0, 1, Menu::doNothing, anyEvent, noStyle),
  FIELD(comFlowControlIndex, "Flow Control", "", 0, 1, 0, 1, Menu::doNothing, anyEvent, noStyle),
  OP("Save", actionComSaveAndExit, enterEvent),
  OP("Back", actionComExitNoSave, enterEvent)
);

MENU(resetConfirmMenu, "Reset Device", Menu::doNothing, Menu::noEvent, Menu::noStyle,
  OP("Confirm Reset", actionResetConfirm, enterEvent),
  OP("Back", actionBack, enterEvent)
);

MENU(wifiMenu, "WiFi", Menu::doNothing, Menu::noEvent, Menu::wrapStyle,
  OP("Status / Web UI", actionWifiStatus, enterEvent),
  OP("Back", actionBack, enterEvent)
);

MENU(brokerMenu, "Broker", Menu::doNothing, Menu::noEvent, Menu::wrapStyle,
  OP("1 Krake PubInv", actionBrokerKrake, enterEvent),
  OP("2 Public Shiftr", actionBrokerPublic, enterEvent),
  OP("Back", actionBack, enterEvent)
);

MENU(muteMenu, "Mute Duration", Menu::doNothing, Menu::noEvent, Menu::wrapStyle,
  FIELD(muteTimeoutMinutes, "Set Duration", "min", 1, 60, 5, 1, actionMuteTimeout, enterEvent, wrapStyle),
  OP("Mute Now", actionMuteNow, enterEvent),
  OP("Unmute", actionUnmuteNow, enterEvent),
  OP("Back", actionBack, enterEvent)
);

MENU(developerMenu, "Developer Mode", Menu::doNothing, Menu::noEvent, Menu::wrapStyle,
  SUBMENU(brokerMenu),
  SUBMENU(comSetupMenu),
  OP("Diagnostics", actionDiagnostics, enterEvent),
  OP("Back", actionBack, enterEvent)
);

MENU(mainMenu, "Settings", Menu::doNothing, Menu::noEvent, Menu::wrapStyle,
  OP("Info", actionInfo, enterEvent),
  SUBMENU(wifiMenu),
  FIELD(volumeDFPlayer, "Volume", "%", 1, 30, 20, 1, action4, enterEvent, wrapStyle),
  SUBMENU(muteMenu),
  SUBMENU(developerMenu),
  SUBMENU(resetConfirmMenu),
  OP("Back", actionSettingsBack, enterEvent)
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
  DBG_PRINT(F("CW: "));
  DBG_PRINTLN(CW);
  reIn.registerEvent(CW ? RotaryEventIn::EventType::ROTARY_CW
                        : RotaryEventIn::EventType::ROTARY_CCW);
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
  if (settingsExitRequested)
  {
    finishReturnToMainPage();
    return;
  }
  if (running_menu)
  {
    drawLcdStatusIconsNow();
  }
}

static void finishReturnToMainPage()
{
  settingsExitRequested = false;
  nav.reset();
  running_menu = false;
  menu_just_exited = false;
  resetLcdUiToMainPage();
  lcd.noBlink();
  lcd.noCursor();
  showMainLcdFrameNow(&Serial);
}

void navigate_to_n_and_execute(int n)
{
  nav.doNav(navCmd(idxCmd, n));
}

void open_settings_menu_at(int n)
{
  reset_menu_navigation();
  navigate_to_n_and_execute(n);
}

void reset_menu_navigation()
{
  running_menu = true;
  nav.reset();
}

void executeAlarmAction(uint8_t actionIndex)
{
  switch (actionIndex)
  {
  case 0:
    action1(eventMask::enterEvent);
    break;
  case 1:
    action2(eventMask::enterEvent);
    break;
  case 2:
    action3(eventMask::enterEvent);
    break;
  case 3:
    action5(eventMask::enterEvent);
    break;
  default:
    break;
  }
}
