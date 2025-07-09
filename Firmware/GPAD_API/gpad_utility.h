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
#define COMPANY_NAME "PubInv "  // For the Broker ID for MQTT 
#define PROG_NAME "GPAD_API "   // This program
#define FIRMWARE_VERSION "0.44 " // Initial Menu implementation 
//#define HARDWARE_VERSION "V0.0.1 "
#define MODEL_NAME "KRAKE_"
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "US"
#define DEVICE_UNDER_TEST "Krake: DFPlayer"  //This is GPAD code, but if it is used in testing...

// THIS IS FOR DEBUGGING
#define LIMIT_POWER_DRAW 1

void printError(Stream *serialport);
void printInstructions(Stream *serialport);
void printAlarmState(Stream *serialport);
#endif
