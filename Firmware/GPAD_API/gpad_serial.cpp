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
#include "robot_api.h"
#include <Arduino.h>

extern bool currentlyMuted;

// We accept maessages up to 128 characters, with 2 characters in front,
// and an end-of-string delimiter makes 131 characters!
const int COMMAND_BUFFER_SIZE = 131;
char buf[COMMAND_BUFFER_SIZE];

const int NUM_PREFICES = 4;
char legal_prefices[NUM_PREFICES] = {'h','s','a','u'};



// This is a trivial "parser". This should probably be moved
// into a separate .cpp file.
/*
This is a simple protocol:
CD\n
where C is an character, and D is a single digit.
*/
void interpretBuffer(char *buf,int rlen,Stream *serialport) {
  if (rlen < 1) {
    printError(serialport);
    return; // no action
  }

  bool found = false;
  for(int i = 0; i < NUM_PREFICES; i++) {
    if (buf[0] == legal_prefices[i]) found = true;
  }
  if (!found) {
    printError(serialport);
    return;
  }
  char C = buf[0];

  serialport->print(F("Command: "));
  serialport->println(C);
  switch (C) {
    case 's':
      serialport->println(F("Muting Case!"));
      currentlyMuted = true;
      break;
    case 'u':
      serialport->println(F("UnMuting Case!"));
      currentlyMuted = false;
      break;
    case 'h': // help
      printInstructions(serialport);
      break;
    case 'a': {
    // In the case of an alarm state, the rest of the buffer is a message.
    // we will read up to 60 characters from this buffer for display on our
    // Arguably when we support mulitple states this will become more complicated.
      char D = buf[1];
      int N = D - '0';
      serialport->println(N);
      char msg[61];
      msg[0] = '\0';
      strcpy(msg, buf+2);
      // This copy loooks uncessary, but is not...we want "alarm"
      // to be a completely independent and abstract function.
      // it should copy the msg buffer
      Serial.print("The MQTT Alarm Message: ");
      Serial.println(msg);      
      alarm((AlarmLevel) N,msg,serialport); //Makes Lamps indicate alarm.
      break;
    }
    default:
      serialport->println(F("Unknown Command"));
      break;
  }
  serialport->print(F("currentlyMuted : "));
  serialport->println(currentlyMuted);
  serialport->println(F("interpret Done"));
//FLE  delay(3000);
}//end interpretBuffer()

void processSerial(Stream *serialport) {
   // Now see if we have a serial command
    int rlen;
    // TODO: This code can probably hang; it needs to have
    // timeouts added!
    if (serialport->available() > 0) {
      // read the incoming bytes:
      int rlen = serialport->readBytesUntil('\n', buf, COMMAND_BUFFER_SIZE);
      // readBytesUntil does not terminate the string!
      buf[rlen] = '\0';
      // prints the received data
      serialport->print(F("I received: "));
      serialport->print(rlen);
      for(int i = 0; i < rlen; i++)
        serialport->print(buf[i]);
      serialport->println();
      interpretBuffer(buf,rlen,serialport);
      // Now "light and scream"appropriately...
      // This does not work on HMWK2 device
      annunciateAlarmLevel(serialport);
      delay(3000);
      printAlarmState(serialport);
     }
}
