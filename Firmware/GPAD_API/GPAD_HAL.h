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

#ifndef GPAD_HAL
#define GPAD_HAL 1
#include <stream.h>
#include <Arduino.h>
#include <PubSubClient.h> 

// On Nov. 5th, 2024, we image 3 different hardware platforms.
// The GPAD exists, and is working: https://www.hardware-x.com/article/S2468-0672(24)00084-1/fulltext
// The "HMWK2" device exists as an intermediate design, and we are working on this.
// The "Krake" has not yet been designed.
// Note: The GPAD is based on an Arduino UNO, but the HMWK2 and KRAKE goes to ESP32.

// Define only ONE of these hardware options.
// #define GPAD 1
#define HMWK 1
// #define KRAKE 1

// Use these to choose the I2C address of LCD
// GPAD device (earlier version of the Krake)
// #define LCD_ADDRESS 0x38 
// Maryville version and Austin version
// #define LCD_ADDRESS 0x27 
// General (Lebanon) Version
#define LCD_ADDRESS 0x3F 

//Pin definitions.  Assign symbolic constant to Arduino pin numbers.
//For more information see: https://www.arduino.cc/en/Tutorial/Foundations/DigitalPins
// This should be done with an "#elif", but I can't get it to work

#if defined(KRAKE)

#define SWITCH_MUTE 35
//#define TONE_PIN 8
#define LIGHT0 12
#define LIGHT1 14
#define LIGHT2 27
#define LIGHT3 26
#define LIGHT4 25
#define LED_BUILTIN 2
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

//#define SWITCH_MUTE 34
#define SWITCH_MUTE 0 //Boot button

#define LED_D9 23
#define LIGHT0 15
#define LIGHT1 4
#define LIGHT2 5
#define LIGHT3 18
#define LIGHT4 19
// The HMWK and Krake use a dev kit LED
#define LED_BUILTIN 2

#endif

#if defined(KRAKE)


#endif

#ifdef GPAD_VERSION1  //The Version 1 PCB.
//#define SS 7                                // nCS aka /SS Input on GPAD Version 1 PCB.

#if defined(HMWK)
// const int LED_D9 = 23;  // Mute1 LED on PMD
#define LED_PIN 23      // for GPAD LIGHT0
#define BUTTON_PIN 2    //GPAD Button to GND,  10K Resistor to +5V.
#else                   // compile for an UNO, for example...
#define LED_PIN PD3     // for GPAD LIGHT0
#define BUTTON_PIN PD2  //GPAD Button to GND,  10K Resistor to +5V.
#endif

#else  //The proof of concept wiring.
#define LED_PIN 7
#define BUTTON_PIN 2  //Button to GND, 10K Resistor to +5V.
#endif


// Define TX and RX pins for UART1 
// For PCB and for Mocking Krake Maryville
//See also Issue# 94
#define TXD1 15
#define RXD1 2
#define UART1_BAUD_RATE 115200
extern HardwareSerial uartSerial1;

// Define TX and RX pins for UART2
// For PCB and for Mocking Krake Maryville
// DFPLayer requries 9600 BPS
#define TXD2 15
#define RXD2 2
#define UART2_BAUD_RATE 9600
extern HardwareSerial uartSerial1;

// SPI Functions....
void setup_spi();
void receive_byte(byte c);
void updateFromSPI();

void unchanged_anunicateAlarmLevel(Stream *serialport);
void annunciateAlarmLevel(Stream *serialport);
void clearLCD(void);
void splashLCD(void);

void interpretBuffer(char *buf, int rlen, Stream *serialport, PubSubClient *client);

// This module has to be initialized and called each time through the superloop
void GPAD_HAL_setup(Stream *serialport);
void GPAD_HAL_loop();
#endif
