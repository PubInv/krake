/* robot_api.h
  header for the "robot" (low-level hardware) api

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

#ifndef ROBOT_API
#define ROBOT_API 1
#include <stream.h>

//Pin definitions.  Assign symbolic constant to Arduino pin numbers.
//For more information see: https://www.arduino.cc/en/Tutorial/Foundations/DigitalPins
#define SWITCH_MUTE 2
#define TONE_PIN 8

#if defined(ESP32)
#define LED_D9 23
#define LIGHT0 15
#define LIGHT1 4
#define LIGHT2 5
#define LIGHT3 18
#define LIGHT4 19
#else
#define LIGHT0 3
#define LIGHT1 5
#define LIGHT2 6
#define LIGHT3 9
#define LIGHT4 7
#endif

// The Krake uses a dev kit LED 
// #define LED_BUILTIN 23
#define LED_BUILTIN 23

void unchanged_anunicateAlarmLevel(Stream *serialport);
void annunciateAlarmLevel(Stream *serialport);
void clearLCD(void);
void splashLCD(void);

// This module has to be initialized and called each time through the superloop
void robot_api_setup(Stream *serialport);
void robot_api_loop();
#endif
