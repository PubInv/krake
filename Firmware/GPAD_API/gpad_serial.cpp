/* gpad_serial.cpp
  implement a serial "protocol" for the GPAD alarms

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

#include "gpad_serial.h"
#include "gpad_utility.h"
#include "alarm_api.h"
#include "GPAD_HAL.h"
#include <Arduino.h>

extern bool currentlyMuted;



// We accept maessages up to 128 characters, with 2 characters in front,
// and an end-of-string delimiter makes 131 characters!
const int COMMAND_BUFFER_SIZE = 131;
char buf[COMMAND_BUFFER_SIZE];

// This is a trivial "parser". This should probably be moved
// into a separate .cpp file.
/*
This is a simple protocol:
CD\n
where C is an character, and D is a single digit.
*/

// Note: The buffer "buf" used here might be more safely made 
// a parameter passed in from the caller.
void processSerial(Stream *debugPort,Stream *inputPort) {
  // Now see if we have a serial command
  int rlen;
  // TODO: This code can probably hang; it needs to have
  // timeouts added!
  if (inputPort->available() > 0) {
    // read the incoming bytes:
    int rlen = inputPort->readBytesUntil('\n', buf, COMMAND_BUFFER_SIZE);
    // readBytesUntil does not terminate the string!
    buf[rlen] = '\0';
    // prints the received data
    debugPort->print(F("I received: "));
    debugPort->print(rlen);
    for (int i = 0; i < rlen; i++)
      debugPort->print(buf[i]);
    debugPort->println();
    interpretBuffer(buf, rlen, inputPort);
    // Now "light and scream"appropriately...
    // This does not work on HMWK2 device
    annunciateAlarmLevel(debugPort);
// removing in an attempt to make faster; reason for adding unknown  - rlr
//    delay(3000);
    printAlarmState(debugPort);
  }
}
