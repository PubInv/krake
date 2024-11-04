/* alarm_api.h
  The header file for the "abstract" alarm api.

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

#ifndef ALARM_API
#define ALARM_API
#include <Stream.h>

enum AlarmLevel { silent, informational, problem, warning, critical, panic };
// const char *AlarmNames[] = { "OK   ","INFO.","PROB.","WARN ","CRIT.","PANIC" };
const int NUM_LEVELS = 6;

const int MAX_MSG_LEN = 80;
const int MAX_BUFFER_SIZE = MAX_MSG_LEN + 1;
typedef struct {
                uint8_t lvl;
                // we will use a null-terminated string!
                char msg[MAX_MSG_LEN+1];
               } AlarmEvent;

int alarm_event(AlarmEvent& event,Stream *serialport);
int alarm(AlarmLevel level,char *str,Stream *serialport);

#endif
