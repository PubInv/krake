/* gpad_utility.cpp
  low-level utility functions needed by every module (presumably) in the GPAD system

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

#include "gpad_utility.h"
#include "alarm_api.h"

extern AlarmLevel currentLevel;
extern bool currentlyMuted;
extern char AlarmMessageBuffer[81];

void printError(Stream *serialport)
{
  serialport->println(F("bad format of command!"));
  printInstructions(serialport);
}
void printInstructions(Stream *serialport)
{
  serialport->println(F("PubInv GPAD: enter command in form CDa (C is a char, D is a digit)"));
}
void printAlarmState(Stream *serialport)
{
  serialport->print(F("Muted: "));
  serialport->println(currentlyMuted ? "YES" : "NO");
  serialport->print(F("LVL: "));
  serialport->println(currentLevel);
  if (strlen(AlarmMessageBuffer) == 0)
  {
    serialport->println(F("No Message."));
  }
  else
  {
    serialport->print(F("Msg: "));
    serialport->println(AlarmMessageBuffer);
  }
}
