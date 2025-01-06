/* gpad_utility.h
  Header files for low-level utility functions needed by every module (presumably) in the GPAD system

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

#ifndef GPAD_UTILITY
#define GPAD_UTILITY 1
#include <Stream.h>

#define PROG_NAME "GPAD_API "
#define FIRMWARE_VERSION "0.25 "
//#define HARDWARE_VERSION "V0.0.1 "
#define MODEL_NAME "KRAKE_"
//#define DEVICE_UNDER_TEST "20240421_LEB1"  //A Serial Number
//#define DEVICE_UNDER_TEST "20240421_LEB3"  //A Serial Number
//#define DEVICE_UNDER_TEST "20240421_USA1"  //A Serial Number
//#define DEVICE_UNDER_TEST "20240421_USA5"  //A Serial Number
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "US"

void printError(Stream *serialport);
void printInstructions(Stream *serialport);
void printAlarmState(Stream *serialport);
#endif
