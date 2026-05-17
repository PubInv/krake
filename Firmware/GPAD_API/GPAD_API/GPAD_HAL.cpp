/* GPAD_HAL.cpp
   The Hardware Abstraction Layer (HAL) (low-level hardware) api

  Copyright (C) 2022 Robert Read

  This program includes free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  See the GNU Affero General Public License for more details.
  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*/

// #include <arduino.h>
#include "GPAD_HAL.h"
#include "alarm_api.h"
#include "gpad_utility.h"
#include <SPI.h>
#include "WiFiManagerOTA.h"
#include "GPAD_menu.h"
#include "mqtt_handler.h"
#include "debug_macros.h"
#include <esp_system.h>
#include <esp_wifi.h>
#include <Preferences.h>
#include <driver/uart.h>
#include <GPAPMessage.h>
#include <stdarg.h>
using namespace gpad_hal;


namespace
{
  Preferences comPrefs;
  const char *COM_PREF_NS = "comcfg";
  const char *COM_PREF_BAUD = "baud";
  const char *COM_PREF_FMT = "fmt";
  const char *COM_PREF_FLOW = "flow";

  const uint8_t SERIAL_FORMAT_COUNT = 1;
  const uint32_t COM_DEFAULT_BAUD_RATE = 9600;
  const uint8_t COM_DEFAULT_SERIAL_FORMAT_INDEX = 0; // 8-N-1
  const ComFlowControlMode COM_DEFAULT_FLOW = COM_FLOW_OFF;

  ComPortConfig comPortConfig = {COM_DEFAULT_BAUD_RATE, COM_DEFAULT_SERIAL_FORMAT_INDEX, COM_DEFAULT_FLOW};

  bool isSupportedBaudRate(uint32_t baudRate)
  {
    switch (baudRate)
    {
    case 1200:
    case 2400:
    case 4800:
    case 9600:
    case 19200:
    case 38400:
    case 57600:
    case 115200:
      return true;
    default:
      return false;
    }
  }

  uint32_t sanitizeBaudRate(uint32_t baudRate)
  {
    return isSupportedBaudRate(baudRate) ? baudRate : COM_DEFAULT_BAUD_RATE;
  }

  uint8_t sanitizeSerialFormatIndex(uint8_t serialFormatIndex)
  {
    return (serialFormatIndex < SERIAL_FORMAT_COUNT) ? serialFormatIndex : COM_DEFAULT_SERIAL_FORMAT_INDEX;
  }

  ComFlowControlMode sanitizeFlowControl(uint8_t flow)
  {
    return (flow == static_cast<uint8_t>(COM_FLOW_RTS_CTS)) ? COM_FLOW_RTS_CTS : COM_FLOW_OFF;
  }

  uint32_t loadComBaudRate() { return sanitizeBaudRate(comPrefs.getUInt(COM_PREF_BAUD, COM_DEFAULT_BAUD_RATE)); }
  uint8_t loadComSerialFormatIndex() { return sanitizeSerialFormatIndex(comPrefs.getUChar(COM_PREF_FMT, COM_DEFAULT_SERIAL_FORMAT_INDEX)); }
  ComFlowControlMode loadComFlowControl() { return sanitizeFlowControl(comPrefs.getUChar(COM_PREF_FLOW, static_cast<uint8_t>(COM_DEFAULT_FLOW))); }

  uart_word_length_t uartDataBitsFromFormatIndex(uint8_t serialFormatIndex)
  {
    (void)serialFormatIndex;
    return UART_DATA_8_BITS;
  }
}

const ComPortConfig &getComPortConfig() { return comPortConfig; }

bool setComPortBaudRate(uint32_t baudRate)
{
  if (!isSupportedBaudRate(baudRate)) return false;
  comPortConfig.baudRate = baudRate;
  comPrefs.putUInt(COM_PREF_BAUD, comPortConfig.baudRate);
  return true;
}

bool setComPortSerialFormatIndex(uint8_t serialFormatIndex)
{
  if (serialFormatIndex >= SERIAL_FORMAT_COUNT) return false;
  comPortConfig.serialFormatIndex = serialFormatIndex;
  comPrefs.putUChar(COM_PREF_FMT, comPortConfig.serialFormatIndex);
  return true;
}

bool setComPortFlowControl(ComFlowControlMode flowControl)
{
  if (flowControl != COM_FLOW_OFF && flowControl != COM_FLOW_RTS_CTS) return false;
  comPortConfig.flowControl = flowControl;
  comPrefs.putUChar(COM_PREF_FLOW, static_cast<uint8_t>(comPortConfig.flowControl));
  return true;
}

void applyComPortConfig(Stream *serialport)
{
  uartSerial1.begin(comPortConfig.baudRate, SERIAL_8N1, RXD1, TXD1);
  uartSerial1.flush();

  const uart_word_length_t dbits = uartDataBitsFromFormatIndex(comPortConfig.serialFormatIndex);
  uart_set_word_length(UART_NUM_1, dbits);
  uart_set_parity(UART_NUM_1, UART_PARITY_DISABLE);
  uart_set_stop_bits(UART_NUM_1, UART_STOP_BITS_1);

  if (comPortConfig.flowControl == COM_FLOW_RTS_CTS)
  {
    uart_set_hw_flow_ctrl(UART_NUM_1, UART_HW_FLOWCTRL_CTS_RTS, 122);
  }
  else
  {
    uart_set_hw_flow_ctrl(UART_NUM_1, UART_HW_FLOWCTRL_DISABLE, 0);
  }

  if (serialport != nullptr)
  {
    serialport->print(F("COM UART1: "));
    serialport->print(comPortConfig.baudRate);
    serialport->print(F(", 8-N-1, flow="));
    serialport->println((comPortConfig.flowControl == COM_FLOW_RTS_CTS) ? F("RTS/CTS") : F("off"));
  }
}

const char *resetReasonToString(esp_reset_reason_t reason);

// Use Serial1 for UART communication
HardwareSerial uartSerial1(1); // For user Serial Port
HardwareSerial uartSerial2(2); // For DFPLayer, audio

#include <DailyStruggleButton.h>
// Time in ms you need to hold down the button to be considered a long press
unsigned int longPressTime = 1000;
// How many times you need to hit the button to be considered a multi-hit
byte multiHitTarget = 2;
// How fast you need to hit all buttons to be considered a multi-hit
unsigned int multiHitTime = 400;

DailyStruggleButton muteButton;
DailyStruggleButton encoderSwitchButton;

extern const char *AlarmNames[];
extern AlarmLevel currentLevel;
extern bool currentlyMuted;
extern char AlarmMessageBuffer[81];
extern unsigned long muteTimeoutEndMillis;
extern bool running_menu;

extern char macAddressString[13];
extern int muteTimeoutMinutes;
extern char currentAlarmId[11];
extern char currentAlarmType[4];
extern PubSubClient client;
extern char mqtt_broker_name[];
extern uint8_t selectedBrokerIndex;
extern uint8_t activeBrokerIndex;
extern uint8_t mqttFailCount;
extern const char *mqttStateDescription(int state);
extern const char *brokerConnectionStateText();
extern bool selectMqttBrokerOption(uint8_t index);
 
// For LCD
//  #include <LiquidCrystal_I2C.h>

// https://github.com/johnrickman/LiquidCrystal_I2C

LiquidCrystal_I2C Real_lcd(LCD_ADDRESS, 20, 4);
LCDWrapper lcd;

#include "DFPlayer.h"

// Setup for buzzer.
// const int BUZZER_TEST_FREQ = 130; // One below middle C3. About 67 db, 3" x 4.875" 8 Ohm speakers no cabinet at 1 Meter.
// const int BUZZER_TEST_FREQ = 260; // Middle C4. About ?? db, 3" x 4.875" 8 Ohm speakers no cabinet at 1 Meter.
// const int BUZZER_TEST_FREQ = 1000; //About 76 db, 3" x 4.875" 8 Ohm speakers no cabinet at 1 Meter.
const int BUZZER_TEST_FREQ = 4000; // Buzzers, 3 V 4kHz 60dB @ 3V, 10 cm.  The specified frequencey for the Version 1 buzzer.

const int BUZZER_LVL_FREQ_HZ[] = {0, 128, 256, 512, 1024, 2048};

// This as an attempt to program recognizable "songs" for each alarm level that accomplish
// both informativeness and urgency mapping. The is is to use an index into the buzzer
// level frequencies above, so we can use an unsigned char. We can break the whole
// sequence into 100ms chunks. A 0 will make a "rest" or a silence.a length of 60 will
// give us a 6-second repeat.

const unsigned int NUM_NOTES = 20;
const int SONGS[][NUM_NOTES] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                                {2, 2, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 2, 2, 0, 0, 0, 0},
                                {3, 3, 3, 0, 3, 3, 3, 3, 0, 3, 3, 3, 0, 3, 3, 3, 0, 0, 0, 0},
                                {4, 0, 4, 0, 4, 0, 4, 0, 0, 0, 4, 0, 4, 0, 4, 0, 4, 0, 0, 0},
                                {4, 4, 2, 0, 4, 4, 2, 0, 4, 4, 2, 0, 4, 4, 2, 0, 4, 4, 2, 0}};

const int LIGHT_LEVEL[][NUM_NOTES] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                      {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                                      {2, 2, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 2, 2, 0, 0, 0, 0},
                                      {3, 3, 3, 0, 3, 3, 3, 3, 0, 3, 3, 3, 0, 3, 3, 3, 0, 0, 0, 0},
                                      {4, 4, 4, 0, 4, 4, 4, 0, 0, 0, 4, 4, 4, 0, 4, 4, 4, 0, 0, 0},
                                      {0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5}};

const unsigned LEN_OF_NOTE_MS = 500;

unsigned long start_of_song = 0;

namespace
{
  const unsigned long ALARM_UI_MIN_INTERVAL_MS = 250;
  const unsigned long ALARM_AUDIO_MIN_INTERVAL_MS = 250;
  const unsigned long ALARM_UI_NORMAL_SETTLE_MS = 25;
  const unsigned long ALARM_UI_BURST_SETTLE_MS = 150;
  const unsigned long DFPLAYER_POLL_INTERVAL_MS = 100;
  const uint8_t ALARM_UI_BURST_REQUEST_COUNT = 3;
  const uint8_t LCD_COLS = 20;
  const uint8_t LCD_ROWS = 4;
  const uint8_t LCD_STATUS_COL = 16;
  const uint8_t LCD_MAIN_WIDTH = LCD_STATUS_COL;
  const uint8_t LCD_MSG_ROWS = 2;
  const size_t MAX_ALARM_TEXT = 160;
  const uint8_t MAX_WRAPPED_LINES = 8;
  const uint8_t ICON_WIFI = 1;
  const uint8_t ICON_VOLUME = 3;
  const uint8_t ICON_MUTE = 4;
  const uint8_t ICON_SETTINGS = 5;
  const uint8_t ALARM_ACTION_COUNT = 4;
  const unsigned long LCD_RENDER_MIN_INTERVAL_MS = 150;

  bool alarmUiUpdatePending = false;
  bool alarmAudioUpdatePending = false;
  AlarmLevel pendingAlarmAudioLevel = silent;
  unsigned long lastAlarmUiRequestMs = 0;
  unsigned long lastAlarmUiUpdateMs = 0;
  unsigned long lastAlarmAudioUpdateMs = 0;
  uint8_t alarmUiPendingRequestCount = 0;
  bool lcdDirty = true;
  bool alarmActionSelectorActive = false;
  uint8_t alarmActionSelection = 0;
  uint8_t alarmQueueCount = 0;
  unsigned long lastLcdRenderMs = 0;
  char alarmText[MAX_ALARM_TEXT] = "";
  char wrappedLines[MAX_WRAPPED_LINES][LCD_COLS + 1];
  uint8_t wrappedLineCount = 0;
  uint8_t alarmPage = 0;
  bool iconFocusActive = false;
  enum LcdFocus : uint8_t
  {
    FOCUS_ALARM_ACTIONS = 0,
    FOCUS_WIFI = 1,
    FOCUS_BROKER = 2,
    FOCUS_MUTE = 3,
    FOCUS_SETTINGS = 4,
  };
  enum LcdPage : uint8_t
  {
    PAGE_MAIN = 0,
    PAGE_WIFI = 1,
    PAGE_BROKER = 2,
    PAGE_MUTE = 3,
    PAGE_INFO = 4,
    PAGE_WIFI_STATUS = 5,
  };
  enum LcdUiState : uint8_t
  {
    MAIN_PAGE = 0,
    SETTINGS_MENU = 1,
    ICON_MENU = 2,
    ALARM_ACTION_SELECT = 3,
    ACTION_FEEDBACK = 4,
    INFO_PAGE = 5,
  };
  LcdFocus lcdFocus = FOCUS_SETTINGS;
  LcdPage lcdPage = PAGE_MAIN;
  LcdUiState lcdUiState = MAIN_PAGE;
  uint8_t lcdPageOption = 0;
  char actionFeedbackText[LCD_COLS + 1] = "";
  unsigned long actionFeedbackStartMs = 0;
  const unsigned long ACTION_FEEDBACK_DURATION_MS = 2500;
  char previousLcdRows[LCD_ROWS][LCD_COLS + 1] = {
      "                    ",
      "                    ",
      "                    ",
      "                    ",
  };

  const char *alarmLevelLabel(AlarmLevel level)
  {
    switch (level)
    {
    case informational:
      return "INFO";
    case problem:
      return "PROB";
    case warning:
      return "WARN";
    case critical:
      return "CRIT";
    case panic:
      return "PANIC";
    case silent:
    default:
      return "OK";
    }
  }

  void markLcdDirty()
  {
    lcdDirty = true;
    alarmUiUpdatePending = true;
    lastAlarmUiRequestMs = millis();
  }

  void setLcdUiState(LcdUiState state)
  {
    lcdUiState = state;
    markLcdDirty();
  }

  bool alarmIsActive()
  {
    return currentLevel != silent;
  }

  uint8_t pageOptionCount()
  {
    switch (lcdPage)
    {
    case PAGE_WIFI:
      return 1;
    case PAGE_BROKER:
      return 3;
    case PAGE_MUTE:
      return 3;
    case PAGE_MAIN:
    default:
      return 0;
    }
  }

  LcdFocus nextFocus(LcdFocus focus, bool clockwise)
  {
    const uint8_t minFocus = alarmIsActive() ? FOCUS_ALARM_ACTIONS : FOCUS_WIFI;
    const uint8_t maxFocus = FOCUS_SETTINGS;
    uint8_t value = static_cast<uint8_t>(focus);
    if (value < minFocus || value > maxFocus)
    {
      value = alarmIsActive() ? FOCUS_ALARM_ACTIONS : FOCUS_WIFI;
    }

    if (clockwise)
    {
      value = (value >= maxFocus) ? minFocus : value + 1;
    }
    else
    {
      value = (value <= minFocus) ? maxFocus : value - 1;
    }
    return static_cast<LcdFocus>(value);
  }

  void clearRowBuffer(char *row)
  {
    memset(row, ' ', LCD_COLS);
    row[LCD_COLS] = '\0';
  }

  void copyText(char *row, uint8_t col, uint8_t width, const char *text)
  {
    if (row == nullptr || text == nullptr || col >= LCD_COLS)
    {
      return;
    }

    uint8_t written = 0;
    while (text[written] != '\0' && written < width && (col + written) < LCD_COLS)
    {
      row[col + written] = text[written];
      written++;
    }
  }

  void clearWrappedLines()
  {
    wrappedLineCount = 0;
    for (uint8_t row = 0; row < MAX_WRAPPED_LINES; row++)
    {
      memset(wrappedLines[row], ' ', LCD_COLS);
      wrappedLines[row][LCD_COLS] = '\0';
    }
  }

  bool copyJsonFieldValue(const char *src, const char *key, char *dst, size_t dstSize)
  {
    if (src == nullptr || key == nullptr || dst == nullptr || dstSize == 0)
    {
      return false;
    }

    char pattern[24];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char *keyPos = strstr(src, pattern);
    if (keyPos == nullptr)
    {
      return false;
    }

    const char *colon = strchr(keyPos + strlen(pattern), ':');
    if (colon == nullptr)
    {
      return false;
    }

    const char *value = colon + 1;
    while (*value == ' ')
    {
      value++;
    }

    const bool quoted = (*value == '"');
    if (quoted)
    {
      value++;
    }

    size_t out = 0;
    for (; *value != '\0' && out + 1 < dstSize; value++)
    {
      if (quoted)
      {
        if (*value == '"')
        {
          break;
        }
        if (*value == '\\' && value[1] != '\0')
        {
          value++;
          switch (*value)
          {
          case 'n':
          case 'r':
          case 't':
            dst[out++] = ' ';
            break;
          default:
            dst[out++] = *value;
            break;
          }
          continue;
        }
      }
      else if (*value == ',' || *value == '}')
      {
        break;
      }

      dst[out++] = *value;
    }
    dst[out] = '\0';
    return out > 0;
  }

  void extractReadableAlarmPayload(const char *src, char *dst, size_t dstSize)
  {
    if (dstSize == 0)
    {
      return;
    }
    dst[0] = '\0';
    if (src == nullptr)
    {
      return;
    }

    const char *keys[] = {"message", "msg", "alarm", "content", "description", "text"};
    for (uint8_t i = 0; i < (sizeof(keys) / sizeof(keys[0])); i++)
    {
      if (copyJsonFieldValue(src, keys[i], dst, dstSize))
      {
        return;
      }
    }

    const char *payload = src;
    const char *lastPipe = strrchr(src, '|');
    if (lastPipe != nullptr && lastPipe[1] != '\0')
    {
      payload = lastPipe + 1;
    }
    else if (strncmp(src, "GPAP:", 5) == 0)
    {
      payload = src + 5;
    }

    snprintf(dst, dstSize, "%s", payload);
  }

  void sanitizeAlarmText(const char *src, char *dst, size_t dstSize)
  {
    if (dstSize == 0)
    {
      return;
    }
    dst[0] = '\0';
    if (src == nullptr)
    {
      return;
    }

    char readable[MAX_ALARM_TEXT];
    extractReadableAlarmPayload(src, readable, sizeof(readable));

    size_t out = 0;
    bool lastWasSpace = true;
    for (size_t in = 0; readable[in] != '\0' && out + 1 < dstSize; in++)
    {
      char c = readable[in];
      if (c == '\r' || c == '\n' || c == '\t')
      {
        c = ' ';
      }
      if (c < 32 || c > 126)
      {
        continue;
      }
      if (c == ' ')
      {
        if (lastWasSpace)
        {
          continue;
        }
        lastWasSpace = true;
      }
      else
      {
        lastWasSpace = false;
      }
      dst[out++] = c;
    }
    while (out > 0 && dst[out - 1] == ' ')
    {
      out--;
    }
    dst[out] = '\0';
  }

  void wrapAlarmTextToLines(const char *text)
  {
    clearWrappedLines();
    if (text == nullptr || text[0] == '\0')
    {
      copyText(wrappedLines[0], 0, LCD_COLS, "No alarm message");
      wrappedLineCount = 1;
      alarmPage = 0;
      return;
    }

    size_t pos = 0;
    const size_t len = strlen(text);
    while (pos < len && wrappedLineCount < MAX_WRAPPED_LINES)
    {
      while (pos < len && text[pos] == ' ')
      {
        pos++;
      }
      if (pos >= len)
      {
        break;
      }

      size_t remaining = len - pos;
      size_t take = (remaining > LCD_COLS) ? LCD_COLS : remaining;
      if (remaining > LCD_COLS)
      {
        int breakAt = -1;
        for (int i = LCD_COLS; i >= 0; i--)
        {
          if (text[pos + i] == ' ')
          {
            breakAt = i;
            break;
          }
        }
        if (breakAt > 4)
        {
          take = static_cast<size_t>(breakAt);
        }
      }

      memcpy(wrappedLines[wrappedLineCount], text + pos, take);
      wrappedLines[wrappedLineCount][LCD_COLS] = '\0';
      wrappedLineCount++;
      pos += take;
    }

    if (wrappedLineCount == 0)
    {
      copyText(wrappedLines[0], 0, LCD_COLS, "No alarm message");
      wrappedLineCount = 1;
    }
    if (pos < len)
    {
      wrappedLines[wrappedLineCount - 1][LCD_COLS - 3] = '.';
      wrappedLines[wrappedLineCount - 1][LCD_COLS - 2] = '.';
      wrappedLines[wrappedLineCount - 1][LCD_COLS - 1] = '.';
    }
    alarmPage = 0;
  }

  uint8_t alarmPageCount()
  {
    if (wrappedLineCount == 0)
    {
      return 1;
    }
    return (wrappedLineCount + (LCD_MSG_ROWS - 1)) / LCD_MSG_ROWS;
  }

  void renderAlarmPage(char rows[LCD_ROWS][LCD_COLS + 1])
  {
    const uint8_t lineIndex = alarmPage * LCD_MSG_ROWS;
    memcpy(rows[1], lineIndex < wrappedLineCount ? wrappedLines[lineIndex] : "                    ", LCD_COLS);
    rows[1][LCD_COLS] = '\0';
    memcpy(rows[2], (lineIndex + 1) < wrappedLineCount ? wrappedLines[lineIndex + 1] : "                    ", LCD_COLS);
    rows[2][LCD_COLS] = '\0';
  }

  bool setAlarmPage(uint8_t page)
  {
    const uint8_t pages = alarmPageCount();
    if (pages <= 1)
    {
      alarmPage = 0;
      return false;
    }
    alarmPage = page % pages;
    return true;
  }

  bool moveAlarmPage(bool clockwise)
  {
    const uint8_t pages = alarmPageCount();
    if (pages <= 1)
    {
      alarmPage = 0;
      return false;
    }
    if (clockwise)
    {
      alarmPage = (alarmPage + 1) % pages;
    }
    else
    {
      alarmPage = (alarmPage == 0) ? (pages - 1) : (alarmPage - 1);
    }
    return true;
  }

  void formatMain(char *row, const char *fmt, ...)
  {
    char temp[LCD_MAIN_WIDTH + 1];
    va_list args;
    va_start(args, fmt);
    vsnprintf(temp, sizeof(temp), fmt, args);
    va_end(args);
    copyText(row, 0, LCD_MAIN_WIDTH, temp);
  }

  void formatFullRow(char *row, const char *fmt, ...)
  {
    char temp[LCD_COLS + 1];
    va_list args;
    va_start(args, fmt);
    vsnprintf(temp, sizeof(temp), fmt, args);
    va_end(args);
    copyText(row, 0, LCD_COLS, temp);
  }

  unsigned long remainingMuteMinutes()
  {
    if (!currentlyMuted || muteTimeoutEndMillis == 0)
    {
      return 0;
    }

    const unsigned long now = millis();
    if ((now - muteTimeoutEndMillis) < 0x80000000UL)
    {
      return 0;
    }

    return ((muteTimeoutEndMillis - now) + 59999UL) / 60000UL;
  }

  void currentSsid(char *dest, size_t destLen)
  {
    if (destLen == 0)
    {
      return;
    }
    dest[0] = '\0';
    wifi_ap_record_t apInfo;
    if (WiFi.status() == WL_CONNECTED && esp_wifi_sta_get_ap_info(&apInfo) == ESP_OK)
    {
      snprintf(dest, destLen, "%s", reinterpret_cast<const char *>(apInfo.ssid));
      return;
    }
    snprintf(dest, destLen, "%s", "Not connected");
  }

  void ipAddressText(char *dest, size_t destLen)
  {
    if (destLen == 0)
    {
      return;
    }
    IPAddress ip = WiFi.localIP();
    if (WiFi.status() != WL_CONNECTED)
    {
      ip = WiFi.softAPIP();
    }
    snprintf(dest, destLen, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
  }

  const char *mqttStatusText()
  {
    if (client.connected())
    {
      return "Connected";
    }
    if (WiFi.status() == WL_CONNECTED)
    {
      return "Waiting";
    }
    return "Offline";
  }

  void filterCopy(char *dest, size_t destLen, const char *src)
  {
    if (destLen == 0)
    {
      return;
    }
    dest[0] = '\0';
    if (src == nullptr)
    {
      return;
    }

    size_t out = 0;
    for (size_t in = 0; src[in] != '\0' && out < (destLen - 1); in++)
    {
      const char c = src[in];
      if (isPrintable(c) || c == ' ')
      {
        dest[out++] = c;
      }
    }
    dest[out] = '\0';
  }

  void setLcdCursorMode(bool enabled, uint8_t col = 19, uint8_t row = 0)
  {
    if (enabled)
    {
      lcd.setCursor(col, row);
      lcd.cursor();
      lcd.blink();
    }
    else
    {
      lcd.noBlink();
      lcd.noCursor();
    }
  }

  uint8_t wifiStatusIcon()
  {
    const wifi_mode_t mode = WiFi.getMode();
    if (mode == WIFI_AP || mode == WIFI_AP_STA)
    {
      return 'A';
    }
    return (WiFi.status() == WL_CONNECTED) ? ICON_WIFI : '_';
  }

  uint8_t brokerStatusIcon()
  {
    if (client.connected())
    {
      return 'B';
    }
    if (WiFi.status() != WL_CONNECTED)
    {
      return '_';
    }
    return (mqttFailCount > 0 || activeBrokerIndex != selectedBrokerIndex) ? '?' : '_';
  }

  uint8_t volumeStatusIcon()
  {
    return currentlyMuted ? ICON_MUTE : ICON_VOLUME;
  }

  void writeStatusIcon(uint8_t value)
  {
    if (value < 8)
    {
      lcd.write(value);
    }
    else
    {
      lcd.print(static_cast<char>(value));
    }
  }

  void installLcdIcons()
  {
    byte wifiIcon[8] = {
        B00000,
        B01110,
        B10001,
        B00100,
        B01010,
        B00000,
        B00100,
        B00000,
    };
    byte volumeIcon[8] = {
        B00001,
        B00011,
        B01111,
        B01111,
        B01111,
        B00011,
        B00001,
        B00000,
    };
    byte muteIcon[8] = {
        B10001,
        B01010,
        B00100,
        B01010,
        B10001,
        B00000,
        B11111,
        B00000,
    };
    byte settingsIcon[8] = {
        B00100,
        B10101,
        B01110,
        B11111,
        B01110,
        B10101,
        B00100,
        B00000,
    };

    Real_lcd.createChar(ICON_WIFI, wifiIcon);
    Real_lcd.createChar(ICON_VOLUME, volumeIcon);
    Real_lcd.createChar(ICON_MUTE, muteIcon);
    Real_lcd.createChar(ICON_SETTINGS, settingsIcon);
  }

  void writeStatusIcons(char rows[LCD_ROWS][LCD_COLS + 1])
  {
    rows[0][LCD_STATUS_COL] = wifiStatusIcon();
    rows[0][LCD_STATUS_COL + 1] = brokerStatusIcon();
    rows[0][LCD_STATUS_COL + 2] = volumeStatusIcon();
    rows[0][LCD_STATUS_COL + 3] = ICON_SETTINGS;

    // The compact status cluster lives on row 0; lower rows keep all 20 cells
    // available for alarm details and the settings/action prompts.
  }

  void writeRowToLcd(uint8_t row, const char *text)
  {
    lcd.setCursor(0, row);
    for (uint8_t col = 0; col < LCD_COLS; col++)
    {
      lcd.write(static_cast<uint8_t>(text[col]));
    }
  }

  void renderRows(char rows[LCD_ROWS][LCD_COLS + 1])
  {
    const unsigned long now = millis();
    if (!lcdDirty && lastLcdRenderMs != 0 && (now - lastLcdRenderMs) < LCD_RENDER_MIN_INTERVAL_MS)
    {
      return;
    }

    bool changed = false;
    for (uint8_t row = 0; row < LCD_ROWS; row++)
    {
      rows[row][LCD_COLS] = '\0';
      if (memcmp(previousLcdRows[row], rows[row], LCD_COLS) != 0)
      {
        writeRowToLcd(row, rows[row]);
        memcpy(previousLcdRows[row], rows[row], LCD_COLS);
        previousLcdRows[row][LCD_COLS] = '\0';
        changed = true;
      }
    }

    if (changed)
    {
      lastLcdRenderMs = now;
    }

    setLcdCursorMode(false);
    if (!running_menu)
    {
      if (lcdUiState == ALARM_ACTION_SELECT)
      {
        const uint8_t actionCols[ALARM_ACTION_COUNT] = {0, 5, 9, 13};
        setLcdCursorMode(true, actionCols[alarmActionSelection], 3);
      }
      else if (lcdUiState == MAIN_PAGE)
      {
        if (iconFocusActive && lcdFocus >= FOCUS_WIFI && lcdFocus <= FOCUS_SETTINGS)
        {
          setLcdCursorMode(true, LCD_STATUS_COL + static_cast<uint8_t>(lcdFocus) - FOCUS_WIFI, 0);
        }
      }
      else if (lcdUiState == ICON_MENU)
      {
        uint8_t optionCol = 0;
        uint8_t optionRow = 2;
        if (lcdPage == PAGE_WIFI)
        {
          optionRow = 3;
        }
        else if (lcdPage == PAGE_BROKER)
        {
          optionRow = (lcdPageOption == 0) ? 1 : (lcdPageOption == 1 ? 2 : 3);
        }
        else if (lcdPage == PAGE_MUTE)
        {
          optionRow = (lcdPageOption == 0) ? 2 : 3;
          if (lcdPageOption == 2)
          {
            optionCol = currentlyMuted ? 9 : 11;
          }
        }
        setLcdCursorMode(true, optionCol, optionRow);
      }
    }
    lcdDirty = false;
  }

  void markAlarmUiAudioPending(bool includeAudio = true)
  {
    alarmUiUpdatePending = true;
    lcdDirty = true;
    if (includeAudio)
    {
      alarmAudioUpdatePending = true;
      pendingAlarmAudioLevel = currentLevel;
    }
    lastAlarmUiRequestMs = millis();
    if (alarmUiPendingRequestCount < 255)
    {
      alarmUiPendingRequestCount++;
    }
  }

  bool isDue(const unsigned long now, const unsigned long lastRun, const unsigned long interval)
  {
    return lastRun == 0 || (now - lastRun) >= interval;
  }
}

// in general, we want tones to last forever, although
// I may implement blinking later.
const unsigned long INF_DURATION = 4294967295;

// Allow indexing to LIGHT[] by symbolic name. So LIGHT0 is first and so on.
int LIGHT[] = {LIGHT0, LIGHT1, LIGHT2, LIGHT3, LIGHT4};
int NUM_LIGHTS = sizeof(LIGHT) / sizeof(LIGHT[0]);

Stream *local_ptr_to_serial;

volatile boolean isReceived_SPI;
volatile byte peripheralReceived;

volatile bool procNewPacket = false;
volatile byte indx = 0;
volatile boolean process;

byte received_signal_raw_bytes[MAX_BUFFER_SIZE];

// Local DEBUG defines,  GPAD_HAL
#define DEBUG 0
// #define DEBUG 1

#if (DEBUG > 0)
Serial.println("Debug defined >0");
#endif

void setup_spi()
{
  Serial.println(F("Starting SPI Peripheral."));
  Serial.print(F("Pin for SS: "));
  Serial.println(SS);

  pinMode(BUTTON_PIN, INPUT); // Setting pin 2 as INPUT
  pinMode(LED_PIN, OUTPUT);   // Setting pin 7 as OUTPUT

  //  SPI.begin();    // IMPORTANT. Do not set SPI.begin for a peripherial device.
  pinMode(SS, INPUT_PULLUP); // Sets SS as input for peripherial
  // Why is this not input?
  pinMode(MOSI, INPUT);  // This works for Peripheral
  pinMode(MISO, OUTPUT); // try this.
  pinMode(SCK, INPUT);   // Sets clock as input
#if defined(GPAD)
  SPCR |= _BV(SPE); // Turn on SPI in Peripheral Mode
  // turn on interrupts
  SPCR |= _BV(SPIE);

  isReceived_SPI = false;
  SPI.attachInterrupt(); // Interuupt ON is set for SPI commnucation
#else
#endif

} // end setup_SPI()

// ISRs
//  This is the original...
//  I plan to add an index to this to handle the full message that we intend to receive.
//  However, I think this also needs a timeout to handle the problem of getting out of synch.
const int SPI_BYTE_TIMEOUT_MS = 200; // we don't get the next byte this fast, we reset.
volatile unsigned long last_byte_ms = 0;

#if defined(HMWK)
// void IRAM_ATTR ISR() {
//    receive_byte(SPDR);
// }
#elif defined(GPAD) // compile for an UNO, for example...
ISR(SPI_STC_vect) // Inerrrput routine function
{
  receive_byte(SPDR);
} // end ISR
#endif

void receive_byte(byte c)
{
  last_byte_ms = millis();
  // byte c = SPDR; // read byte from SPI Data Register
  if (indx < sizeof received_signal_raw_bytes)
  {
    received_signal_raw_bytes[indx] = c; // save data in the next index in the array received_signal_raw_bytes
    indx = indx + 1;
  }
  if (indx >= sizeof received_signal_raw_bytes)
  {
    process = true;
  }
}

void updateFromSPI()
{
  if (DEBUG > 0)
  {
    if (process)
    {
      Serial.println("process true!");
    }
  }
  if (process)
  {

    AlarmEvent event;
    event.lvl = (AlarmLevel)received_signal_raw_bytes[0];
    for (int i = 0; i < MAX_MSG_LEN; i++)
    {
      event.msg[i] = (char)received_signal_raw_bytes[1 + i];
    }

    if (DEBUG > 1)
    {
      Serial.print(F("LVL: "));
      Serial.println(event.lvl);
      Serial.println(event.msg);
    }
    int prevLevel = alarm((AlarmLevel)event.lvl, event.msg, &Serial);
    if (prevLevel != event.lvl)
    {
      requestAlarmRefresh(&Serial);
    }
    else
    {
      unchanged_anunicateAlarmLevel(&Serial);
    }

    indx = 0;
    process = false;
  }
}

// Have to get a serialport here

// void myCallback(byte buttonEvent) {
void encoderSwitchCallback(byte buttonEvent)
{
  switch (buttonEvent)
  {
  case onPress:
    // Do something...
    local_ptr_to_serial->println(F("ENCODER_SWITCH onPress"));
    // currentlyMuted = !currentlyMuted;
    // start_of_song = millis();
    // annunciateAlarmLevel(local_ptr_to_serial);
    // printAlarmState(local_ptr_to_serial);

    if (running_menu)
    {
      registerRotaryEncoderPress();
    }
    else if (!alarmActionSelectorHandlePress())
    {
      setLcdUiState(SETTINGS_MENU);
      reset_menu_navigation();
    }
    break;
  case onRelease:
    // Do nothing...
    local_ptr_to_serial->println(F("ENCODER_SWITCH onRelease"));
    break;
  case onHold:
    // Do nothing...
    // local_ptr_to_serial->println(F("ENCODER_SWITCH onHold"));
    break;
    // onLongPress is indidcated when you hold onto the button
  // more than longPressTime in milliseconds
  case onLongPress:
    DBG_PRINT(F("ENCODER_SWITCH Button Long Pressed For "));
    DBG_PRINT(longPressTime);
    DBG_PRINTLN(F("ms"));
    returnToMainPage();
    break;

  // onMultiHit is indicated when you hit the button
  // multiHitTarget times within multihitTime in milliseconds
  case onMultiHit:
    DBG_PRINT(F("Encoder Switch Button Pressed "));
    DBG_PRINT(multiHitTarget);
    DBG_PRINT(F(" times in "));
    DBG_PRINT(multiHitTime);
    DBG_PRINTLN(F("ms"));
    break;
  default:
    DBG_PRINT(F("Encoder Switch buttonEvent but not recognized case: "));
    DBG_PRINTLN(buttonEvent);
    break;
  }
}

// Have to get a serialport here
// void myCallback(byte buttonEvent) {
void muteButtonCallback(byte buttonEvent)
{
  switch (buttonEvent)
  {
  case onPress:
    // Do something...
    local_ptr_to_serial->println(F("SWITCH_MUTE onPress"));
    if (isMuted())
    {
      setMuted(false);
      clearMuteTimeout();
      local_ptr_to_serial->println(F("Manual unmute."));
    }
    else
    {
      setMuteTimeoutMinutes((unsigned long)muteTimeoutMinutes);
      local_ptr_to_serial->print(F("Muted for "));
      local_ptr_to_serial->print(muteTimeoutMinutes);
      local_ptr_to_serial->println(F(" minute(s)."));
      
    }
    start_of_song = millis();
    requestAlarmRefresh(local_ptr_to_serial);
    printAlarmState(local_ptr_to_serial);
    break;
  case onRelease:
    // Do nothing...
    local_ptr_to_serial->println(F("SWITCH_MUTE onRelease"));
    break;
  case onHold:
    // Do nothing...
    local_ptr_to_serial->println(F("SWITCH_MUTE onHold"));
    break;
    // onLongPress is indidcated when you hold onto the button
  // more than longPressTime in milliseconds
  case onLongPress:
    DBG_PRINT(F("SWITCH_MUTE Long Pressed For "));
    DBG_PRINT(longPressTime);
    DBG_PRINTLN(F("ms"));
    break;

  // onMultiHit is indicated when you hit the button
  // multiHitTarget times within multihitTime in milliseconds
  case onMultiHit:
    DBG_PRINT(F("Button Pressed "));
    DBG_PRINT(multiHitTarget);
    DBG_PRINT(F(" times in "));
    DBG_PRINT(multiHitTime);
    DBG_PRINTLN(F("ms"));
    break;
  default:
    DBG_PRINT(F("Mute buttonEvent but not recognized case: "));
    DBG_PRINTLN(buttonEvent);
    break;
  }
}

void GPAD_HAL_setup(Stream *serialport, wifi_mode_t wifiMode, IPAddress &deviceIp)
{
  // Setup and present LCD splash screen
  // Setup the SWITCH_MUTE
  // Setup the SWITCH_ENCODER
  // Print instructions on DEBUG serial port

  lcd.init(&Real_lcd);
  local_ptr_to_serial = serialport;
  Wire.begin();
  
  Real_lcd.init();
  installLcdIcons();
  
  
#if (DEBUG > 0)
  serialport->println(F("Clear LCD"));
#endif
  clearLCD();
  delay(100);
#if (DEBUG > 0)
  serialport->println(F("Start LCD splash"));
#endif

  splashLCD(wifiMode, deviceIp);

#if (DEBUG > 0)
  serialport->println(F("EndLCD splash"));
#endif

  // Setup GPIO pins, Mute and lights
  pinMode(SWITCH_MUTE, INPUT_PULLUP);    // The SWITCH_MUTE is different on Atmega vs ESP32.  Is this redundant?
  pinMode(SWITCH_ENCODER, INPUT_PULLUP); // The SWITCH_ENCODER is new to Krake. Is this redundant?

  for (int i = 0; i < NUM_LIGHTS; i++)
  {
#if (DEBUG > 0)
    serialport->print(LIGHT[i]);
    serialport->print(", ");
#endif
    pinMode(LIGHT[i], OUTPUT);
    // Rob trying to prevent resets
    // This is necessary on SN#3
    digitalWrite(LIGHT[i], LOW);
  }
  serialport->println("");

  muteButton.set(SWITCH_MUTE, muteButtonCallback);
  muteButton.enableLongPress(longPressTime);
  muteButton.enableMultiHit(multiHitTime, multiHitTarget);

  // SW4.set(GPIO_SW4, SendEmergMessage, INT_PULL_UP);
  //   encoderSwitchButton.set(SWITCH_ENCODER, encoderSwitchCallback, INT_PULL_UP);
  encoderSwitchButton.set(SWITCH_ENCODER, encoderSwitchCallback);
  encoderSwitchButton.enableLongPress(longPressTime);
  encoderSwitchButton.enableMultiHit(multiHitTime, multiHitTarget);

  printInstructions(serialport);
  AlarmMessageBuffer[0] = '\0';

  // digitalWrite(LED_BUILTIN, LOW);   // turn the LED off at end of setup

#if !defined(HMWK) // On Homework2, LCD goes blank early
  comPrefs.begin(COM_PREF_NS, false);
  comPortConfig.baudRate = loadComBaudRate();
  comPortConfig.serialFormatIndex = loadComSerialFormatIndex();
  comPortConfig.flowControl = loadComFlowControl();

  applyComPortConfig(serialport);

#if (DEBUG > 0)
  serialport->println(F("uartSerial1 Setup"));
#endif
#endif

  // Here initialize the UART2
  pinMode(RXD2, INPUT_PULLUP);
  uartSerial2.begin(UART2_BAUD_RATE, SERIAL_8N1, RXD2, TXD2); // UART setup
  uartSerial2.flush();
#if (DEBUG > 0)
  serialport->println(F("uartSerial2 Setup"));
#endif
} // end GPAD_HAL_setup()

// This routine should be refactored so that it only "interprets"
// the character buffer and returns an "abstract" command to be acted on
// elseshere. This will allow us to remove the PubSubClient from the this file,
// the Hardware Abstraction Layer.

class CharBufferPrint : public Print
{
public:
  CharBufferPrint(char *buffer, size_t capacity)
      : _buffer(buffer), _capacity(capacity), _pos(0)
  {
    if (_capacity > 0) _buffer[0] = '\0';
  }

  size_t write(uint8_t ch) override
  {
    if (ch == '\0') return 1;

    if (_pos + 1 < _capacity)
    {
      _buffer[_pos++] = (char)ch;
      _buffer[_pos] = '\0';
    }

    return 1;
  }

private:
  char *_buffer;
  size_t _capacity;
  size_t _pos;
};

void printAndPublishStatusLine(Stream *serialport, PubSubClient *mqttClient, const char *line)
{
  serialport->println(line != nullptr ? line : "");
  if (mqttClient != nullptr && mqttClient->connected())
  {
    publishAck(mqttClient, line);
  }
}

void formatUptime(char *dest, size_t destLen)
{
  if (destLen == 0)
  {
    return;
  }
  const unsigned long totalSeconds = millis() / 1000UL;
  const unsigned long hours = totalSeconds / 3600UL;
  const unsigned long minutes = (totalSeconds % 3600UL) / 60UL;
  const unsigned long seconds = totalSeconds % 60UL;
  snprintf(dest, destLen, "%02lu:%02lu:%02lu", hours, minutes, seconds);
}

void printSystemInfo(Stream *serialport, PubSubClient *mqttClient)
{
  char value[80];
  printAndPublishStatusLine(serialport, mqttClient, "=== KRAKE SYSTEM INFO ===");
  snprintf(value, sizeof(value), "KRAKE-%.6s", macAddressString + 6);
  char line[128];
  snprintf(line, sizeof(line), "SN: %s", value);
  printAndPublishStatusLine(serialport, mqttClient, line);
  snprintf(line, sizeof(line), "FW: %s", FIRMWARE_VERSION);
  printAndPublishStatusLine(serialport, mqttClient, line);
  ipAddressText(value, sizeof(value));
  snprintf(line, sizeof(line), "IP: %s", value);
  printAndPublishStatusLine(serialport, mqttClient, line);
  snprintf(line, sizeof(line), "MAC: %s", macAddressString);
  printAndPublishStatusLine(serialport, mqttClient, line);
  currentSsid(value, sizeof(value));
  snprintf(line, sizeof(line), "SSID: %s", value);
  printAndPublishStatusLine(serialport, mqttClient, line);
  snprintf(line, sizeof(line), "Broker: %s", mqtt_broker_name);
  printAndPublishStatusLine(serialport, mqttClient, line);
  snprintf(line, sizeof(line), "MQTT: %s", brokerConnectionStateText());
  printAndPublishStatusLine(serialport, mqttClient, line);
  snprintf(line, sizeof(line), "Heap: %lu", static_cast<unsigned long>(ESP.getFreeHeap()));
  printAndPublishStatusLine(serialport, mqttClient, line);
  formatUptime(value, sizeof(value));
  snprintf(line, sizeof(line), "Uptime: %s", value);
  printAndPublishStatusLine(serialport, mqttClient, line);
  printAndPublishStatusLine(serialport, mqttClient, "=========================");
}

void showStatusLCD(AlarmLevel level, bool muted, char *msg);

void interpretBuffer(char *buf, int rlen, Stream *serialport, PubSubClient *client)
{
  if (buf == nullptr || serialport == nullptr || rlen < 1)
  {
    if (serialport != nullptr)
    {
      printError(serialport);
    }
    return; // no action
  }

  const char command = buf[0];
  if (command == '\0')
  {
    printError(serialport);
    return;
  }
  serialport->print(F("Command: "));
  serialport->printf("%c\n", command);

  switch (command)
  {
  case 's':
  {
    serialport->println(F("Muting Case!"));
    setMuted(true);
    break;
  }
  case 'u':
  {
    serialport->println(F("UnMuting Case!"));
    setMuted(false);
    break;
  }
  case 'h':
  {
    printInstructions(serialport);
    break;
  }
  case 'a':
  {
    auto gpMessage = gpap_message::GPAPMessage::deserialize(buf, (size_t)rlen);

    if (gpMessage.getMessageType() != gpap_message::MessageType::ALARM)
    {
      serialport->println(F("GPAP alarm parse failed."));
      printError(serialport);
      return;
    }

    const auto &alarmMessage = gpMessage.getAlarmMessage();

    strncpy(currentAlarmId, "", sizeof(currentAlarmId));
    strncpy(currentAlarmType, "", sizeof(currentAlarmType));

    CharBufferPrint idWriter(currentAlarmId, sizeof(currentAlarmId));
    CharBufferPrint typeWriter(currentAlarmType, sizeof(currentAlarmType));

    const auto &messageId = alarmMessage.getMessageId();
    if (messageId.state == gpap_message::alarm::AlarmMessage::PossibleMessageId::State::Some)
    {
      messageId.contents.printTo(idWriter);
    }

    const auto &typeDesignator = alarmMessage.getTypeDesignator();
    if (typeDesignator.state == gpap_message::alarm::AlarmMessage::PossibleTypeDesignator::State::Some)
    {
      typeDesignator.contents.printTo(typeWriter);
    }
    int N = static_cast<char>(alarmMessage.getAlarmLevel()) - '0';

    if (N < 0 || N >= NUM_LEVELS)
    {
      serialport->println(F("Invalid GPAP alarm severity."));
      printError(serialport);
      return;
    }

    char msg[MAX_BUFFER_SIZE];
    CharBufferPrint msgWriter(msg, sizeof(msg));
    alarmMessage.getAlarmContent().printTo(msgWriter);

    serialport->print(F("GPAP Alarm Level: "));
    serialport->println(N);

    serialport->print(F("GPAP Alarm Content: "));
    serialport->println(msg);

    alarm((AlarmLevel)N, msg, serialport);
    break;
  }
  case 'I':
  case 'i':
  {
    printSystemInfo(serialport, client);
    break;
  }
  case 'R':
  case 'r':
  {
    serialport->println(F("Software reset requested."));
    if (client != nullptr && client->connected())
    {
      publishAck(client, "Software reset requested.");
    }
    showActionFeedback("System restarting");
    requestAlarmRefresh(serialport, false);
    showStatusLCD(currentLevel, currentlyMuted, AlarmMessageBuffer);
    delay(100);
    ESP.restart();
    break;
  }
  default:
  {
    printError(serialport);
    break;
  }
  }
  serialport->print(F("currentlyMuted : "));
  serialport->println(currentlyMuted);
  serialport->println(F("interpret Done"));
  // FLE  delay(3000);
} // end interpretBuffer()

void muteTimeoutWatchdog(Stream *serialport)
{
  // Watchdog for timed mute: when duration expires, force unmute and re-annunciate.
  if (isMuted() && serviceMuteTimeout())
  {
    serialport->println(F("Mute timeout expired. Auto-unmuting."));
    requestAlarmRefresh(serialport);
  }
}

void serviceAlarmUiAudio(Stream *serialport)
{
  if (serialport == nullptr)
  {
    return;
  }

  const unsigned long now = millis();

  static unsigned long lastDfPlayerPollMs = 0;
  if (isDue(now, lastDfPlayerPollMs, DFPLAYER_POLL_INTERVAL_MS))
  {
    lastDfPlayerPollMs = now;
#if ENABLE_DFPLAYER
    dfPlayerUpdate();
#endif
  }

  if (!alarmUiUpdatePending && !alarmAudioUpdatePending)
  {
    return;
  }

  // Leave the menu display in control while the user is navigating.  The latest
  // alarm state remains pending and will be restored when the menu exits.
  if (running_menu)
  {
    return;
  }

  const unsigned long settleMs = (alarmUiPendingRequestCount >= ALARM_UI_BURST_REQUEST_COUNT)
                                     ? ALARM_UI_BURST_SETTLE_MS
                                     : ALARM_UI_NORMAL_SETTLE_MS;
  if ((now - lastAlarmUiRequestMs) < settleMs)
  {
    return;
  }

  if (alarmUiUpdatePending && isDue(now, lastAlarmUiUpdateMs, ALARM_UI_MIN_INTERVAL_MS))
  {
    alarmUiUpdatePending = false;
    alarmUiPendingRequestCount = 0;
    lastAlarmUiUpdateMs = now;
    showStatusLCD(currentLevel, currentlyMuted, AlarmMessageBuffer);
  }

  if (alarmAudioUpdatePending && isDue(now, lastAlarmAudioUpdateMs, ALARM_AUDIO_MIN_INTERVAL_MS))
  {
    alarmAudioUpdatePending = false;
    lastAlarmAudioUpdateMs = now;

    const AlarmLevel audioLevel = pendingAlarmAudioLevel;

    if (audioLevel <= 0)
    {
      serialport->println(F("Silent level: skipping DFPlayer playback."));
    }
    else if (currentlyMuted)
    {
      serialport->println(F("Muted: skipping DFPlayer playback."));
    }
    else
    {
      serialport->println(F("dfPlayer.play"));
      serialport->println(audioLevel);
      playNotBusyLevel(audioLevel);
    }
  }
}

void GPAD_HAL_loop()
{
  muteButton.poll();
  encoderSwitchButton.poll();
#if defined(GPAD) // FLE??? Why is this conditional compile?
  muteButton.poll();
#endif

  muteTimeoutWatchdog(local_ptr_to_serial);
  static unsigned long lastDashboardRefreshMs = 0;
  const unsigned long now = millis();
  if (lcdUiState == ACTION_FEEDBACK && (now - actionFeedbackStartMs) >= ACTION_FEEDBACK_DURATION_MS)
  {
    actionFeedbackText[0] = '\0';
    setLcdUiState(MAIN_PAGE);
  }
  if (!running_menu && !alarmUiUpdatePending && isDue(now, lastDashboardRefreshMs, ALARM_UI_MIN_INTERVAL_MS))
  {
    lastDashboardRefreshMs = now;
    alarmUiUpdatePending = true;
    lastAlarmUiRequestMs = now;
  }
  serviceAlarmUiAudio(local_ptr_to_serial);
}

bool isAlarmActionSelectorActive()
{
  return lcdUiState == ALARM_ACTION_SELECT;
}

const char *lcdUiStateName()
{
  switch (lcdUiState)
  {
  case MAIN_PAGE:
    return "MAIN_PAGE";
  case SETTINGS_MENU:
    return "SETTINGS_MENU";
  case ICON_MENU:
    return "ICON_MENU";
  case ALARM_ACTION_SELECT:
    return "ALARM_ACTION_SELECT";
  case ACTION_FEEDBACK:
    return "ACTION_FEEDBACK";
  case INFO_PAGE:
    return "INFO_PAGE";
  default:
    return "UNKNOWN";
  }
}

void drawLcdStatusIconsNow()
{
#if ENABLE_LCD_UI
  lcd.setCursor(LCD_STATUS_COL, 0);
  writeStatusIcon(wifiStatusIcon());
  writeStatusIcon(brokerStatusIcon());
  writeStatusIcon(volumeStatusIcon());
  lcd.write(static_cast<uint8_t>(ICON_SETTINGS));
  lcd.noBlink();
  lcd.noCursor();
#endif
}

void noteLcdQueueMessageReceived()
{
  markLcdDirty();
}

void resetLcdUiToMainPage()
{
  lcdPage = PAGE_MAIN;
  lcdPageOption = 0;
  alarmActionSelectorActive = false;
  iconFocusActive = false;
  actionFeedbackText[0] = '\0';
  if (alarmIsActive())
  {
    lcdFocus = FOCUS_ALARM_ACTIONS;
    alarmActionSelection = 0;
  }
  else
  {
    lcdFocus = FOCUS_WIFI;
  }
  setLcdUiState(MAIN_PAGE);
}

void showAlarmActions()
{
  if (!alarmIsActive())
  {
    return;
  }
  lcdPage = PAGE_MAIN;
  alarmActionSelectorActive = true;
  iconFocusActive = false;
  lcdFocus = FOCUS_ALARM_ACTIONS;
  if (lcdUiState != ALARM_ACTION_SELECT)
  {
    alarmActionSelection = 0;
  }
  setLcdUiState(ALARM_ACTION_SELECT);
}

void showActionFeedback(const char *msg)
{
  snprintf(actionFeedbackText, sizeof(actionFeedbackText), "%s", msg != nullptr ? msg : "");
  actionFeedbackStartMs = millis();
  alarmActionSelectorActive = false;
  iconFocusActive = false;
  setLcdUiState(ACTION_FEEDBACK);
}

void showInfoPage()
{
  lcdPage = PAGE_INFO;
  lcdPageOption = 0;
  alarmActionSelectorActive = false;
  iconFocusActive = false;
  setLcdUiState(INFO_PAGE);
  requestAlarmRefresh(local_ptr_to_serial, false);
}

void showWifiStatusPage()
{
  lcdPage = PAGE_WIFI_STATUS;
  lcdPageOption = 0;
  alarmActionSelectorActive = false;
  iconFocusActive = false;
  setLcdUiState(INFO_PAGE);
  requestAlarmRefresh(local_ptr_to_serial, false);
}

void executeSelectedAlarmAction()
{
  const uint8_t selectedAction = alarmActionSelection;
  const uint8_t actionIndex = (selectedAction == 0) ? 2 : (selectedAction == 1 ? 0 : (selectedAction == 2 ? 1 : 3));
  alarmActionSelection = 0;
  alarmActionSelectorActive = false;
  executeAlarmAction(actionIndex);
  switch (selectedAction)
  {
  case 0:
    showActionFeedback("Alarm shelved");
    break;
  case 1:
    showActionFeedback("Alarm acknowledged");
    break;
  case 2:
    showActionFeedback("Alarm dismissed");
    break;
  case 3:
    showActionFeedback("Alarm completed");
    break;
  default:
    break;
  }
}

bool alarmActionSelectorHandleRotation(bool clockwise)
{
  if (lcdUiState == INFO_PAGE && lcdPage == PAGE_INFO)
  {
    if (clockwise)
    {
      lcdPageOption = (lcdPageOption + 1) % 3;
    }
    else
    {
      lcdPageOption = (lcdPageOption == 0) ? 2 : (lcdPageOption - 1);
    }
    markLcdDirty();
    requestAlarmRefresh(local_ptr_to_serial, false);
    return true;
  }

  if (lcdUiState == ICON_MENU)
  {
    const uint8_t count = pageOptionCount();
    if (count > 0)
    {
      if (clockwise)
      {
        lcdPageOption = (lcdPageOption + 1) % count;
      }
      else
      {
        lcdPageOption = (lcdPageOption == 0) ? (count - 1) : (lcdPageOption - 1);
      }
    }
    markLcdDirty();
    requestAlarmRefresh(local_ptr_to_serial, false);
    return true;
  }

  if (alarmIsActive())
  {
    if (lcdUiState == MAIN_PAGE && iconFocusActive && lcdFocus >= FOCUS_WIFI && lcdFocus <= FOCUS_SETTINGS)
    {
      iconFocusActive = true;
      lcdFocus = nextFocus(lcdFocus, clockwise);
      markLcdDirty();
      requestAlarmRefresh(local_ptr_to_serial, false);
      return true;
    }

    if (lcdUiState == MAIN_PAGE && !iconFocusActive)
    {
      if (moveAlarmPage(clockwise))
      {
        markLcdDirty();
        requestAlarmRefresh(local_ptr_to_serial, false);
        return true;
      }
    }

    const bool wasSelectingAlarmAction = (lcdUiState == ALARM_ACTION_SELECT);
    showAlarmActions();
    if (!wasSelectingAlarmAction)
    {
      requestAlarmRefresh(local_ptr_to_serial, false);
      return true;
    }
    if (clockwise)
    {
      if (alarmActionSelection >= (ALARM_ACTION_COUNT - 1))
      {
        alarmActionSelectorActive = false;
        iconFocusActive = true;
        lcdFocus = FOCUS_WIFI;
        setLcdUiState(MAIN_PAGE);
      }
      else
      {
        alarmActionSelection++;
      }
    }
    else
    {
      if (alarmActionSelection == 0)
      {
        alarmActionSelectorActive = false;
        iconFocusActive = true;
        lcdFocus = FOCUS_SETTINGS;
        setLcdUiState(MAIN_PAGE);
      }
      else
      {
        alarmActionSelection--;
      }
    }
    markLcdDirty();
    requestAlarmRefresh(local_ptr_to_serial, false);
    return true;
  }

  if (currentLevel == silent)
  {
    lcdFocus = nextFocus(lcdFocus, clockwise);
    iconFocusActive = true;
    alarmActionSelectorActive = false;
    markLcdDirty();
    requestAlarmRefresh(local_ptr_to_serial, false);
    return true;
  }

  return false;
}

bool alarmActionSelectorHandlePress()
{
  if (lcdUiState == INFO_PAGE)
  {
    returnToMainPage();
    return true;
  }

  if (lcdUiState == ACTION_FEEDBACK)
  {
    returnToMainPage();
    return true;
  }

  if (lcdUiState == ICON_MENU)
  {
    if (lcdPage == PAGE_WIFI)
    {
      if (lcdPageOption == 0)
      {
        returnToMainPage();
      }
    }
    else if (lcdPage == PAGE_BROKER)
    {
      if (lcdPageOption == 0)
      {
        resetLcdUiToMainPage();
        showActionFeedback(selectMqttBrokerOption(1) ? "Broker selected" : "Broker failed");
      }
      else if (lcdPageOption == 1)
      {
        resetLcdUiToMainPage();
        showActionFeedback(selectMqttBrokerOption(0) ? "Broker selected" : "Broker failed");
      }
      else
      {
        returnToMainPage();
      }
    }
    else if (lcdPage == PAGE_MUTE)
    {
      if (lcdPageOption == 0)
      {
        resetLcdUiToMainPage();
        setLcdUiState(SETTINGS_MENU);
        open_settings_menu_at(3);
      }
      else if (lcdPageOption == 1)
      {
        if (currentlyMuted)
        {
          clearMuteTimeout();
          setMuted(false);
        }
        else
        {
          setMuteTimeoutMinutes((unsigned long)muteTimeoutMinutes);
        }
      }
      else
      {
        returnToMainPage();
      }
    }
    markLcdDirty();
    requestAlarmRefresh(local_ptr_to_serial, false);
    return true;
  }

  if (alarmIsActive() && lcdUiState == MAIN_PAGE && !iconFocusActive)
  {
    showAlarmActions();
    requestAlarmRefresh(local_ptr_to_serial, false);
    return true;
  }

  if (alarmIsActive() && lcdUiState != ALARM_ACTION_SELECT && lcdFocus == FOCUS_ALARM_ACTIONS)
  {
    showAlarmActions();
    requestAlarmRefresh(local_ptr_to_serial, false);
    return true;
  }

  if (lcdFocus == FOCUS_WIFI)
  {
    lcdPage = PAGE_WIFI;
    lcdPageOption = 0;
    iconFocusActive = false;
    setLcdUiState(ICON_MENU);
  }
  else if (lcdFocus == FOCUS_BROKER)
  {
    lcdPage = PAGE_BROKER;
    lcdPageOption = 0;
    iconFocusActive = false;
    setLcdUiState(ICON_MENU);
  }
  else if (lcdFocus == FOCUS_MUTE)
  {
    lcdPage = PAGE_MUTE;
    lcdPageOption = 0;
    iconFocusActive = false;
    setLcdUiState(ICON_MENU);
  }
  else if (lcdFocus == FOCUS_SETTINGS)
  {
    resetLcdUiToMainPage();
    iconFocusActive = false;
    setLcdUiState(SETTINGS_MENU);
    reset_menu_navigation();
  }
  else if (lcdUiState != ALARM_ACTION_SELECT)
  {
    return false;
  }
  else
  {
    executeSelectedAlarmAction();
  }
  markLcdDirty();
  requestAlarmRefresh(local_ptr_to_serial, false);
  return true;
}

/* Assumes LCD has been initilized
   Turns off Back Light
   Clears display
   Turns on back light.
*/
void clearLCD(void)
{
  lcd.noBacklight();
  lcd.clear();
  for (uint8_t row = 0; row < LCD_ROWS; row++)
  {
    clearRowBuffer(previousLcdRows[row]);
  }
  lcdDirty = true;
}

// Splash a message so we can tell the LCD is working
void splashLCD(wifi_mode_t wifiMode, const IPAddress &deviceIp)
{
  lcd.init(); // initialize the lcd
  // Print a message to the LCD.
  // #if (!LIMIT_POWER_DRAW)
  lcd.backlight();
  // #else
  //   lcd.noBacklight();
  // #endif

  // Line 0
  lcd.setCursor(0, 0);
  lcd.print(MODEL_NAME);
  // lcd.print(DEVICE_UNDER_TEST);
  //  lcd.setCursor(3, 1);
  lcd.print(PROG_NAME);
  lcd.print(" ");
  lcd.print(FIRMWARE_VERSION);

  // Line 1
  lcd.setCursor(0, 1);
  switch (wifiMode)
  {
  case wifi_mode_t::WIFI_MODE_AP:
    lcd.print("AP");
    break;
  case wifi_mode_t::WIFI_MODE_STA:
    lcd.print("STA");
    break;
  }

  lcd.print(": ");
  deviceIp.printTo(lcd);

  // Line 2
  lcd.setCursor(0, 2);
  lcd.print(F(__DATE__ " " __TIME__));

  // Leave row 3 empty; normal UI state owns the bottom row.
}
bool printable(char c)
{
  return isPrintable(c) || (c == ' ');
}
// Remove unwanted characters....
void filter_control_chars(char *msg)
{
  if (msg == nullptr)
  {
    return;
  }

  size_t len = strlen(msg);
  if (len >= MAX_BUFFER_SIZE)
  {
    len = MAX_BUFFER_SIZE - 1;
    msg[len] = '\0';
  }

  char buff[MAX_BUFFER_SIZE];
  memcpy(buff, msg, len);
  buff[len] = '\0';
  int k = 0;
  for (int i = 0; i < len; i++)
  {
    char c = buff[i];
    if (printable(c))
    {
      msg[k] = c;
      k++;
    }
  }
  msg[k] = '\0';
}

void renderWifiPage(char rows[LCD_ROWS][LCD_COLS + 1])
{
  char ssid[21];
  char ip[21];
  currentSsid(ssid, sizeof(ssid));
  ipAddressText(ip, sizeof(ip));

  if (WiFi.status() == WL_CONNECTED)
  {
    formatFullRow(rows[0], "WiFi:%.15s", ssid);
    formatFullRow(rows[1], "IP:%s", ip);
    formatFullRow(rows[2], "Open Web UI");
    formatFullRow(rows[3], "%cBack Open Web UI", lcdPageOption == 0 ? '>' : ' ');
  }
  else
  {
    formatFullRow(rows[0], "WiFi Setup");
    formatFullRow(rows[1], "AP:Krake-Setup");
    formatFullRow(rows[2], "Go:%s", ip);
    formatFullRow(rows[3], "%cBack Open Web UI", lcdPageOption == 0 ? '>' : ' ');
  }
}

void renderBrokerPage(char rows[LCD_ROWS][LCD_COLS + 1])
{
  formatFullRow(rows[0], "Broker Select");
  formatFullRow(rows[1], "%c1 Krake PubInv", lcdPageOption == 0 ? '>' : ' ');
  formatFullRow(rows[2], "%c2 Public Shiftr", lcdPageOption == 1 ? '>' : ' ');
  formatFullRow(rows[3], "%cBack", lcdPageOption == 2 ? '>' : ' ');
}

void renderMutePage(char rows[LCD_ROWS][LCD_COLS + 1])
{
  const unsigned long muteMinutes = remainingMuteMinutes();
  formatFullRow(rows[0], "Mute");
  formatFullRow(rows[1], "Mute set:%lu min", currentlyMuted ? muteMinutes : (unsigned long)muteTimeoutMinutes);
  formatFullRow(rows[2], "%cMute settings", lcdPageOption == 0 ? '>' : ' ');
  formatFullRow(rows[3], "%c%s  %cBack",
                lcdPageOption == 1 ? '>' : ' ',
                currentlyMuted ? "Unmute" : "Mute now",
                lcdPageOption == 2 ? '>' : ' ');
}

void renderInfoPage(char rows[LCD_ROWS][LCD_COLS + 1])
{
  char ip[21];
  char ssid[21];
  ipAddressText(ip, sizeof(ip));
  currentSsid(ssid, sizeof(ssid));
  char serialNumber[21];
  snprintf(serialNumber, sizeof(serialNumber), "KRAKE-%.6s", macAddressString + 6);

  formatFullRow(rows[0], "Info %u/3", static_cast<unsigned>(lcdPageOption + 1));
  if (lcdPageOption == 0)
  {
    formatFullRow(rows[1], "SN:%s", serialNumber);
    formatFullRow(rows[2], "FW:%s", FIRMWARE_VERSION);
  }
  else if (lcdPageOption == 1)
  {
    formatFullRow(rows[1], "IP:%s", ip);
    formatFullRow(rows[2], "MAC:%s", macAddressString);
  }
  else
  {
    formatFullRow(rows[1], "SSID:%.15s", ssid);
    formatFullRow(rows[2], "MQTT:%.15s", brokerConnectionStateText());
  }
  formatFullRow(rows[3], "Back");
}

void renderWifiStatusPage(char rows[LCD_ROWS][LCD_COLS + 1])
{
  char ssid[21];
  char ip[21];
  currentSsid(ssid, sizeof(ssid));
  ipAddressText(ip, sizeof(ip));

  if (WiFi.status() == WL_CONNECTED)
  {
    formatFullRow(rows[0], "WiFi:%.15s", ssid);
    formatFullRow(rows[1], "IP:%s", ip);
    formatFullRow(rows[2], "Open Web UI");
    formatFullRow(rows[3], "Press: Back");
  }
  else
  {
    formatFullRow(rows[0], "WiFi Setup");
    formatFullRow(rows[1], "AP:Krake-Setup");
    formatFullRow(rows[2], "Go:%s", ip);
    formatFullRow(rows[3], "Press: Back");
  }
}

void showStatusLCD(AlarmLevel level, bool muted, char *msg)
{
  char rows[LCD_ROWS][LCD_COLS + 1];
  for (uint8_t row = 0; row < LCD_ROWS; row++)
  {
    clearRowBuffer(rows[row]);
  }

  if (level != silent)
  {
    lcd.backlight();
  }
  else
  {
    lcd.backlight();
  }

  alarmQueueCount = (level == silent) ? 0 : 1;

  if (lcdUiState == INFO_PAGE)
  {
    if (lcdPage == PAGE_WIFI_STATUS)
    {
      renderWifiStatusPage(rows);
    }
    else
    {
      renderInfoPage(rows);
    }
  }
  else if (lcdUiState == ICON_MENU && lcdPage == PAGE_WIFI)
  {
    renderWifiPage(rows);
  }
  else if (lcdUiState == ICON_MENU && lcdPage == PAGE_BROKER)
  {
    renderBrokerPage(rows);
  }
  else if (lcdUiState == ICON_MENU && lcdPage == PAGE_MUTE)
  {
    renderMutePage(rows);
  }
  else if (level == silent)
  {
    alarmText[0] = '\0';
    clearWrappedLines();
    alarmPage = 0;
    if (lcdUiState == ALARM_ACTION_SELECT)
    {
      lcdUiState = MAIN_PAGE;
    }
    formatMain(rows[0], "PAQ:0");
    formatMain(rows[1], "System OK");
    if (currentlyMuted)
    {
      formatFullRow(rows[2], "Vol:%02d Mute:%lum", volumeDFPlayer, remainingMuteMinutes());
    }
    else
    {
      formatFullRow(rows[2], "Vol:%02d Mute:Off", volumeDFPlayer);
    }
    alarmActionSelectorActive = false;
    if (lcdFocus == FOCUS_ALARM_ACTIONS)
    {
      lcdFocus = FOCUS_SETTINGS;
    }
  }
  else
  {
    char cleanMsg[MAX_ALARM_TEXT];
    sanitizeAlarmText(msg, cleanMsg, sizeof(cleanMsg));
    char displayText[MAX_ALARM_TEXT];
    if (currentAlarmType[0] != '\0')
    {
      snprintf(displayText, sizeof(displayText), "%s %s %s", alarmLevelLabel(level), currentAlarmType, cleanMsg);
    }
    else
    {
      snprintf(displayText, sizeof(displayText), "%s %s", alarmLevelLabel(level), cleanMsg);
    }

    if (strcmp(alarmText, displayText) != 0)
    {
      snprintf(alarmText, sizeof(alarmText), "%s", displayText);
      wrapAlarmTextToLines(alarmText);
      iconFocusActive = false;
    }

    const uint8_t pages = alarmPageCount();
    if (alarmPage >= pages)
    {
      setAlarmPage(0);
    }

    if (alarmQueueCount > 1)
    {
      if (pages > 1)
      {
        formatMain(rows[0], "PAQ:+ P%u/%u", static_cast<unsigned>(alarmPage + 1), static_cast<unsigned>(pages));
      }
      else
      {
        formatMain(rows[0], "PAQ:+ NEXT");
      }
    }
    else
    {
      if (pages > 1)
      {
        formatMain(rows[0], "PAQ:1 P%u/%u", static_cast<unsigned>(alarmPage + 1), static_cast<unsigned>(pages));
      }
      else
      {
        formatMain(rows[0], "PAQ:1");
      }
    }

    renderAlarmPage(rows);

    if (lcdUiState == ALARM_ACTION_SELECT)
    {
      formatFullRow(rows[3], "SHLV ACK DIS CMP");
    }

    if (lcdUiState == ACTION_FEEDBACK)
    {
      formatFullRow(rows[3], "%s", actionFeedbackText);
    }
  }

  if (level == silent && lcdUiState == ACTION_FEEDBACK)
  {
    formatFullRow(rows[3], "%s", actionFeedbackText);
  }

  (void)muted;
  writeStatusIcons(rows);
  renderRows(rows);
}

// This operation is idempotent if there is no change in the abstract state.
void set_light_level(int lvl)
{
  for (int i = 0; i < lvl; i++)
  {
    digitalWrite(LIGHT[i], HIGH);
  }
  for (int i = lvl; i < NUM_LIGHTS; i++)
  {
    digitalWrite(LIGHT[i], LOW);
  }
}
void unchanged_anunicateAlarmLevel(Stream *serialport)
{
  unsigned long m = millis();
  unsigned long time_in_song = m - start_of_song;
  unsigned char note = time_in_song / (unsigned long)LEN_OF_NOTE_MS;
  //   serialport->print("note: ");
  //   serialport->println(note);
  if (note >= NUM_NOTES)
  {
    note = 0;
    start_of_song = m;
  }
  unsigned char light_lvl = LIGHT_LEVEL[currentLevel][note];
  set_light_level(light_lvl);
  // TODO: Change this to our device types
// #if !defined(HMWK)
#if defined(GPAD)
  if (!currentlyMuted)
  {
    unsigned char note_lvl = SONGS[currentLevel][note];

    //   serialport->print("note lvl");
    //   serialport->println(note_lvl);
    tone(TONE_PIN, BUZZER_LVL_FREQ_HZ[note_lvl], INF_DURATION);
  }
  else
  {
    noTone(TONE_PIN);
  }
#endif
}
void restoreAlarmLevel(Stream *serialport)
{
  if (serialport == nullptr)
  {
    return;
  }

  markAlarmUiAudioPending(false);
}

void requestAlarmRefresh(Stream *serialport, bool includeAudio)
{
  if (serialport == nullptr)
  {
    return;
  }

  start_of_song = millis();
  markAlarmUiAudioPending(includeAudio);
}

void showMainLcdFrameNow(Stream *serialport)
{
  if (serialport == nullptr)
  {
    return;
  }

  resetLcdUiToMainPage();
  alarmUiUpdatePending = false;
  alarmUiPendingRequestCount = 0;
  lastAlarmUiUpdateMs = millis();
  for (uint8_t row = 0; row < LCD_ROWS; row++)
  {
    previousLcdRows[row][0] = '\0';
  }
  showStatusLCD(currentLevel, currentlyMuted, AlarmMessageBuffer);
}

void annunciateAlarmLevel(Stream *serialport)
{
  if (serialport == nullptr)
  {
    return;
  }

  // Keep the immediate work cheap so MQTT callbacks and serial/SPI handlers can
  // return quickly.  LCD redraw and DFPlayer commands are coalesced in
  // serviceAlarmUiAudio(), which is run later from GPAD_HAL_loop().
  start_of_song = millis();
  unchanged_anunicateAlarmLevel(serialport);
  markAlarmUiAudioPending();
}

GPAD_API::GPAD_API(SemanticVersion version)
    : version(version)
{
}

const SemanticVersion &GPAD_API::getVersion() const
{
  return this->version;
}

SemanticVersion::SemanticVersion(uint8_t major, uint8_t minor, uint8_t patch)
    : major(major),
      minor(minor),
      patch(patch)
{
}

std::string SemanticVersion::toString() const
{
  std::string versionString = std::to_string(this->major);
  versionString.push_back('.');
  versionString.append(std::to_string(this->minor));
  versionString.push_back('.');
  versionString.append(std::to_string(this->patch));

  return versionString;
}
const char *resetReasonToString(esp_reset_reason_t reason)
{
  switch (reason)
  {
  case ESP_RST_UNKNOWN: return "UNKNOWN";
  case ESP_RST_POWERON: return "POWERON";
  case ESP_RST_EXT: return "EXTERNAL";
  case ESP_RST_SW: return "SOFTWARE";
  case ESP_RST_PANIC: return "PANIC";
  case ESP_RST_INT_WDT: return "INT_WDT";
  case ESP_RST_TASK_WDT: return "TASK_WDT";
  case ESP_RST_WDT: return "OTHER_WDT";
  case ESP_RST_DEEPSLEEP: return "DEEPSLEEP";
  case ESP_RST_BROWNOUT: return "BROWNOUT";
  case ESP_RST_SDIO: return "SDIO";
  default: return "UNMAPPED";
  }
}
