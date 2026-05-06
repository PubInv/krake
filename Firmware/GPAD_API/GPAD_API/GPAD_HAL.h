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

#ifndef GPAD_HAL_H
#define GPAD_HAL_H
#include <Stream.h>
// #include <Arduino.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>

// On Nov. 5th, 2024, we image 3 different hardware platforms.
// The GPAD exists, and is working: https://www.hardware-x.com/article/S2468-0672(24)00084-1/fulltext
// The "HMWK2" device exists as an intermediate design, and we are working on this.
// The "Krake" has not yet been designed.
// Note: The GPAD is based on an Arduino UNO, but the HMWK2 and KRAKE goes to ESP32.

// Define only ONE of these hardware options.
// #define GPAD 1
// #define HMWK 1
#define KRAKE 1

// Use these to choose the I2C address of LCD
// GPAD device (earlier version of the Krake)
#define LCD_ADDRESS 0x38
// Maryville version and Austin version
// #define LCD_ADDRESS 0x27
// General (Lebanon) Version
// #define LCD_ADDRESS 0x3F

// Pin definitions.  Assign symbolic constant to Arduino pin numbers.
// For more information see: https://www.arduino.cc/en/Tutorial/Foundations/DigitalPins
//  This should be done with an "#elif", but I can't get it to work

#if defined(KRAKE)

#define SWITCH_MUTE 35
// #define TONE_PIN 8
#define LIGHT0 12
#define LIGHT1 14
#define LIGHT2 27
#define LIGHT3 26
#define LIGHT4 25
#define LED_BUILTIN 13
#define SWITCH_ENCODER 34 // Center switch aka button Normaly high.
#endif

#if defined(GPAD)

#define SWITCH_MUTE 2
#define TONE_PIN 8
#define LIGHT0 3
#define LIGHT1 5
#define LIGHT2 6
#define LIGHT3 9
#define LIGHT4 7
#define LED_BUILTIN 13
#endif

// This should be done with an "#elif", but I can't get it to work
#if defined(HMWK)

// #define SWITCH_MUTE 34
#define SWITCH_MUTE 0 // Boot button
#define LED_D9 23
#define LIGHT0 15
#define LIGHT1 4
#define LIGHT2 5
#define LIGHT3 18
#define LIGHT4 19
// The HMWK use a dev kit LED
#define LED_BUILTIN 2

#endif

#ifdef GPAD_VERSION1 // The Version 1 PCB.
// #define SS 7                                // nCS aka /SS Input on GPAD Version 1 PCB.

#if defined(HMWK)
// const int LED_D9 = 23;  // Mute1 LED on PMD
#define LED_PIN 23     // for GPAD LIGHT0
#define BUTTON_PIN 2   // GPAD Button to GND,  10K Resistor to +5V.
#else                  // compile for an UNO, for example...
#define LED_PIN PD3    // for GPAD LIGHT0
#define BUTTON_PIN PD2 // GPAD Button to GND,  10K Resistor to +5V.
#endif

#else // The proof of concept wiring.
#define LED_PIN 7
#define BUTTON_PIN 2 // Button to GND, 10K Resistor to +5V.
#endif

// Define TX and RX pins for UART1
// For PCB and for Mocking Krake Maryville
// See also Issue# 94
#define TXD1 2
#define RXD1 15
#define UART1_BAUD_RATE 115200
extern HardwareSerial uartSerial1;

// Define TX and RX pins for UART2
// For PCB and for Mocking Krake Maryville
// DFPLayer requries 9600 BPS
#define TXD2 17
#define RXD2 16
#define UART2_BAUD_RATE 9600
extern HardwareSerial uartSerial2;


enum ComFlowControlMode : uint8_t
{
  COM_FLOW_OFF = 0,
  COM_FLOW_RTS_CTS = 1,
};

struct ComPortConfig
{
  uint32_t baudRate;
  uint8_t serialFormatIndex;
  ComFlowControlMode flowControl;
};

const ComPortConfig &getComPortConfig();
bool setComPortBaudRate(uint32_t baudRate);
bool setComPortSerialFormatIndex(uint8_t serialFormatIndex);
bool setComPortFlowControl(ComFlowControlMode flowControl);
void applyComPortConfig(Stream *serialport);

namespace gpad_hal
{

  static const uint8_t API_MAJOR_VERSION = 0;
  static const uint8_t API_MINOR_VERSION = 1;
  static const uint8_t API_PATCH_VERSION = 0;

  /**
   * SemanticVersion stores a version following the "semantic versioning" convention
   * defined here: https://semver.org/
   *
   * In summary:
   *  - Major version defines breaking an incompatible changes with different versions
   *  - Minor version adds new functionality in a backwards capability
   *    ie v2.4.1 and v2.5.1 are still compatible but v2.5.1 may have additional functionality
   *  - Patch version simply addresses bugs with no new features again in a backwards
   *    compatible way
   */
  class SemanticVersion
  {
  public:
    SemanticVersion(uint8_t major, uint8_t minor, uint8_t patch);

    std::string toString() const;

  private:
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
  };

  class GPAD_API
  {
  public:
    GPAD_API(SemanticVersion version);

    const SemanticVersion &getVersion() const;

  private:
    const SemanticVersion version;
  };

  const GPAD_API gpadApi = GPAD_API(SemanticVersion(API_MAJOR_VERSION, API_MINOR_VERSION, API_PATCH_VERSION));
}



class LCDWrapper : public Print
{
public :
  static const uint8_t LCD_COLS = 20;
  static const uint8_t LCD_ROWS = 4;

  LCDWrapper() : _LCD(nullptr), _cursorCol(0), _cursorRow(0)
  {
    resetMirror();
  }

  virtual size_t write(uint8_t b) override
  {
    if (_LCD == nullptr)
    {
      return 0;
    }

    if (b == '\r')
    {
      return 1;
    }

    if (b == '\n')
    {
      _cursorCol = 0;
      _cursorRow = (_cursorRow + 1) % LCD_ROWS;
      return 1;
    }

    _LCD->write(b);
    if (_cursorRow < LCD_ROWS && _cursorCol < LCD_COLS)
    {
      _lcdMirror[_cursorRow][_cursorCol] = static_cast<char>(b);
    }

    if (_cursorCol < LCD_COLS)
    {
      _cursorCol++;
    }

    return 1;
  }
  void init(LiquidCrystal_I2C* _lcd)
  {
    _LCD = _lcd;
    resetMirror();
  }
  void init()
  {
    _LCD->init();
    resetMirror();
  }
  void clear(){
    _LCD->clear();
    resetMirror();
  }
  void backlight(){
    _LCD->backlight();
  }
  void noBacklight(){
    _LCD->noBacklight();
  }
  void setCursor(int16_t col, int16_t row){
    _LCD->setCursor(col, row);
    _cursorCol = constrain(col, 0, LCD_COLS - 1);
    _cursorRow = constrain(row, 0, LCD_ROWS - 1);
  }
  void noBlink(){
    _LCD->noBlink();
  }
  void blink(){
    _LCD->blink();
  }
  void cursor(){
    _LCD->cursor();
  }
  void noCursor(){
    _LCD->noCursor();
  }
  String line(uint8_t row) const
  {
    if (row >= LCD_ROWS)
    {
      return String("");
    }
    return String(_lcdMirror[row]);
  }
private:
  void resetMirror()
  {
    for (uint8_t row = 0; row < LCD_ROWS; row++)
    {
      for (uint8_t col = 0; col < LCD_COLS; col++)
      {
        _lcdMirror[row][col] = ' ';
      }
      _lcdMirror[row][LCD_COLS] = '\0';
    }
    _cursorCol = 0;
    _cursorRow = 0;
  }

  LiquidCrystal_I2C* _LCD;
  char _lcdMirror[LCD_ROWS][LCD_COLS + 1];
  uint8_t _cursorCol;
  uint8_t _cursorRow;
};

// SPI Functions....
void setup_spi();
void receive_byte(byte c);
void updateFromSPI();

void restoreAlarmLevel(Stream *serialport);
void unchanged_anunicateAlarmLevel(Stream *serialport);
void annunciateAlarmLevel(Stream *serialport);
void clearLCD(void);
void showMqttStatusLCD(bool connected);
void splashLCD(wifi_mode_t wifiMode, IPAddress &deviceIp);

void interpretBuffer(char *buf, int rlen, Stream *serialport, PubSubClient *client);

// This module has to be initialized and called each time through the superloop
void GPAD_HAL_setup(Stream *serialport, wifi_mode_t wifiMode, IPAddress &deviceIp);
void muteTimeoutWatchdog(Stream *serialport);
void GPAD_HAL_loop();

extern LiquidCrystal_I2C Real_lcd;
extern LCDWrapper lcd;
#endif
