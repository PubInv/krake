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
// #include <Arduino.h>

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
void processSerial(Stream *debugPort, Stream *inputPort, PubSubClient *client)
{
  if (debugPort == nullptr || inputPort == nullptr)
  {
    return;
  }

  static size_t writeIndex = 0;
  bool processedCommand = false;

  while (inputPort->available() > 0 && !processedCommand)
  {
    const int raw = inputPort->read();
    if (raw < 0)
    {
      break;
    }

    const char c = (char)raw;
    if (c == '\r')
    {
      continue;
    }

    if (c == '\n')
    {
      const int rlen = (int)writeIndex;
      buf[rlen] = '\0';

      debugPort->print(F("I received: "));
      debugPort->print(rlen);
      for (int i = 0; i < rlen; i++)
      {
        debugPort->print(buf[i]);
      }
      debugPort->println();

      if (rlen > 0)
      {
        interpretBuffer(buf, rlen, debugPort, client);
        annunciateAlarmLevel(debugPort);
        printAlarmState(debugPort);
        processedCommand = true;
      }
      writeIndex = 0;
      continue;
    }

    if (writeIndex < (COMMAND_BUFFER_SIZE - 1))
    {
      buf[writeIndex++] = c;
    }
    else
    {
      // Overflow guard: reset buffer if a line grows too long.
      writeIndex = 0;
      printError(debugPort);
      processedCommand = true;
    }
  }

  // Keep the scheduler/WDT serviced during burst traffic.
  if (processedCommand)
  {
    yield();
  }
}
