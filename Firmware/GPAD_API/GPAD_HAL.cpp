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

//#include <arduino.h>
#include "GPAD_HAL.h"
#include "alarm_api.h"
#include "gpad_utility.h"
#include <SPI.h>

extern IPAddress myIP;

// Use Serial1 for UART communication
HardwareSerial uartSerial1(1);  //For user Serial Port
HardwareSerial uartSerial2(2);  //For DFPLayer, audio


#include <DailyStruggleButton.h>
// Time in ms you need to hold down the button to be considered a long press
unsigned int longPressTime = 1000;
// How many times you need to hit the button to be considered a multi-hit
byte multiHitTarget = 2;
// How fast you need to hit all buttons to be considered a multi-hit
unsigned int multiHitTime = 400;

DailyStruggleButton muteButton;
DailyStruggleButton encoderSwitchButton;


extern const char *AlarmNames[];
extern AlarmLevel currentLevel;
extern bool currentlyMuted;
extern char AlarmMessageBuffer[81];

extern char macAddressString[13];

// TODO: Remove this; for explanation only
extern char publish_Ack_Topic[17];

#include <PubSubClient.h>  // From library https://github.com/knolleary/pubsubclient

extern PubSubClient client;

//For LCD
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(LCD_ADDRESS, 20, 4);

#include "DFPlayer.h"

//Setup for buzzer.
//const int BUZZER_TEST_FREQ = 130; // One below middle C3. About 67 db, 3" x 4.875" 8 Ohm speakers no cabinet at 1 Meter.
//const int BUZZER_TEST_FREQ = 260; // Middle C4. About ?? db, 3" x 4.875" 8 Ohm speakers no cabinet at 1 Meter.
//const int BUZZER_TEST_FREQ = 1000; //About 76 db, 3" x 4.875" 8 Ohm speakers no cabinet at 1 Meter.
const int BUZZER_TEST_FREQ = 4000;  // Buzzers, 3 V 4kHz 60dB @ 3V, 10 cm.  The specified frequencey for the Version 1 buzzer.

const int BUZZER_LVL_FREQ_HZ[] = { 0, 128, 256, 512, 1024, 2048 };

// This as an attempt to program recognizable "songs" for each alarm level that accomplish
// both informativeness and urgency mapping. The is is to use an index into the buzzer
// level frequencies above, so we can use an unsigned char. We can break the whole
// sequence into 100ms chunks. A 0 will make a "rest" or a silence.a length of 60 will
// give us a 6-second repeat.

const unsigned int NUM_NOTES = 20;
const int SONGS[][NUM_NOTES] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                 { 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
                                 { 2, 2, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 2, 2, 0, 0, 0, 0 },
                                 { 3, 3, 3, 0, 3, 3, 3, 3, 0, 3, 3, 3, 0, 3, 3, 3, 0, 0, 0, 0 },
                                 { 4, 0, 4, 0, 4, 0, 4, 0, 0, 0, 4, 0, 4, 0, 4, 0, 4, 0, 0, 0 },
                                 { 4, 4, 2, 0, 4, 4, 2, 0, 4, 4, 2, 0, 4, 4, 2, 0, 4, 4, 2, 0 } };

const int LIGHT_LEVEL[][NUM_NOTES] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                       { 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
                                       { 2, 2, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 2, 2, 0, 0, 0, 0 },
                                       { 3, 3, 3, 0, 3, 3, 3, 3, 0, 3, 3, 3, 0, 3, 3, 3, 0, 0, 0, 0 },
                                       { 4, 4, 4, 0, 4, 4, 4, 0, 0, 0, 4, 4, 4, 0, 4, 4, 4, 0, 0, 0 },
                                       { 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5 } };

const unsigned LEN_OF_NOTE_MS = 500;

unsigned long start_of_song = 0;



// in general, we want tones to last forever, although
// I may implement blinking later.
const unsigned long INF_DURATION = 4294967295;

//Allow indexing to LIGHT[] by symbolic name. So LIGHT0 is first and so on.
int LIGHT[] = { LIGHT0, LIGHT1, LIGHT2, LIGHT3, LIGHT4 };
int NUM_LIGHTS = sizeof(LIGHT) / sizeof(LIGHT[0]);

Stream *local_ptr_to_serial;


volatile boolean isReceived_SPI;
volatile byte peripheralReceived;

volatile bool procNewPacket = false;
volatile byte indx = 0;
volatile boolean process;

byte received_signal_raw_bytes[MAX_BUFFER_SIZE];

// Local DEBUG defines,  GPAD_HAL
#define DEBUG 0
//#define DEBUG 1

#if (DEBUG > 0)
Serial.println("Debug defined >0")
#endif

  const int NUM_PREFICES = 5;
char legal_prefices[NUM_PREFICES] = { 'h', 's', 'a', 'u', 'i' };


void setup_spi() {
  Serial.println(F("Starting SPI Peripheral."));
  Serial.print(F("Pin for SS: "));
  Serial.println(SS);

  pinMode(BUTTON_PIN, INPUT);  // Setting pin 2 as INPUT
  pinMode(LED_PIN, OUTPUT);    // Setting pin 7 as OUTPUT

  //  SPI.begin();    // IMPORTANT. Do not set SPI.begin for a peripherial device.
  pinMode(SS, INPUT_PULLUP);  //Sets SS as input for peripherial
  // Why is this not input?
  pinMode(MOSI, INPUT);   //This works for Peripheral
  pinMode(MISO, OUTPUT);  //try this.
  pinMode(SCK, INPUT);    //Sets clock as input
#if defined(GPAD)
  SPCR |= _BV(SPE);  //Turn on SPI in Peripheral Mode
  // turn on interrupts
  SPCR |= _BV(SPIE);

  isReceived_SPI = false;
  SPI.attachInterrupt();  //Interuupt ON is set for SPI commnucation
#else
#endif

}  //end setup_SPI()

//ISRs
// This is the original...
// I plan to add an index to this to handle the full message that we intend to receive.
// However, I think this also needs a timeout to handle the problem of getting out of synch.
const int SPI_BYTE_TIMEOUT_MS = 200;  // we don't get the next byte this fast, we reset.
volatile unsigned long last_byte_ms = 0;

#if defined(HMWK)
// void IRAM_ATTR ISR() {
//    receive_byte(SPDR);
// }
#elif defined(GPAD)  // compile for an UNO, for example...
ISR(SPI_STC_vect)  //Inerrrput routine function
{
  receive_byte(SPDR);
}  //end ISR
#endif



void receive_byte(byte c) {
  last_byte_ms = millis();
  // byte c = SPDR; // read byte from SPI Data Register
  if (indx < sizeof received_signal_raw_bytes) {
    received_signal_raw_bytes[indx] = c;  // save data in the next index in the array received_signal_raw_bytes
    indx = indx + 1;
  }
  if (indx >= sizeof received_signal_raw_bytes) {
    process = true;
  }
}


void updateFromSPI() {
  if (DEBUG > 0) {
    if (process) {
      Serial.println("process true!");
    }
  }
  if (process) {

    AlarmEvent event;
    event.lvl = (AlarmLevel)received_signal_raw_bytes[0];
    for (int i = 0; i < MAX_MSG_LEN; i++) {
      event.msg[i] = (char)received_signal_raw_bytes[1 + i];
    }

    if (DEBUG > 1) {
      Serial.print(F("LVL: "));
      Serial.println(event.lvl);
      Serial.println(event.msg);
    }
    int prevLevel = alarm((AlarmLevel)event.lvl, event.msg, &Serial);
    if (prevLevel != event.lvl) {
      annunciateAlarmLevel(&Serial);
    } else {
      unchanged_anunicateAlarmLevel(&Serial);
    }

    indx = 0;
    process = false;
  }
}

// Have to get a serialport here

//void myCallback(byte buttonEvent) {
void encoderSwitchCallback(byte buttonEvent) {
  switch (buttonEvent) {
    case onPress:
      // Do something...
      local_ptr_to_serial->println(F("ENCODER_SWITCH onPress"));
      // currentlyMuted = !currentlyMuted;
      // start_of_song = millis();
      // annunciateAlarmLevel(local_ptr_to_serial);
      // printAlarmState(local_ptr_to_serial);
      break;
    case onRelease:
      // Do nothing...
      local_ptr_to_serial->println(F("ENCODER_SWITCH onRelease"));
      break;
    case onHold:
      // Do nothing...
      //local_ptr_to_serial->println(F("ENCODER_SWITCH onHold"));
      break;
      // onLongPress is indidcated when you hold onto the button
    // more than longPressTime in milliseconds
    case onLongPress:
      Serial.print("ENCODER_SWITCH Button Long Pressed For ");
      Serial.print(longPressTime);
      Serial.println("ms");
      break;

    // onMultiHit is indicated when you hit the button
    // multiHitTarget times within multihitTime in milliseconds
    case onMultiHit:
      Serial.print("Encoder Switch Button Pressed ");
      Serial.print(multiHitTarget);
      Serial.print(" times in ");
      Serial.print(multiHitTime);
      Serial.println("ms");
      break;
    default:
      Serial.print("Encoder Switch buttonEvent but not reckognized case: ");
      Serial.println(buttonEvent);
      break;
  }
}

// Have to get a serialport here
//void myCallback(byte buttonEvent) {
void muteButtonCallback(byte buttonEvent) {
  switch (buttonEvent) {
    case onPress:
      // Do something...
      local_ptr_to_serial->println(F("SWITCH_MUTE onPress"));
      currentlyMuted = !currentlyMuted;
      start_of_song = millis();
      annunciateAlarmLevel(local_ptr_to_serial);
      printAlarmState(local_ptr_to_serial);
      break;
    case onRelease:
      // Do nothing...
      local_ptr_to_serial->println(F("SWITCH_MUTE onRelease"));
      break;
    case onHold:
      // Do nothing...
      local_ptr_to_serial->println(F("SWITCH_MUTE onHold"));
      break;
      // onLongPress is indidcated when you hold onto the button
    // more than longPressTime in milliseconds
    case onLongPress:
      Serial.print("SWITCH_MUTE Long Pressed For ");
      Serial.print(longPressTime);
      Serial.println("ms");
      break;

    // onMultiHit is indicated when you hit the button
    // multiHitTarget times within multihitTime in milliseconds
    case onMultiHit:
      Serial.print("Button Pressed ");
      Serial.print(multiHitTarget);
      Serial.print(" times in ");
      Serial.print(multiHitTime);
      Serial.println("ms");
      break;
    default:
      Serial.print("Mute buttonEvent but not reckognized case: ");
      Serial.println(buttonEvent);
      break;
  }
}

void GPAD_HAL_setup(Stream *serialport) {
  //Setup and present LCD splash screen
  //Setup the SWITCH_MUTE
  //Setup the SWITCH_ENCODER
  //Print instructions on DEBUG serial port

  local_ptr_to_serial = serialport;
  Wire.begin();
  lcd.init();
#if (DEBUG > 0)
  serialport->println(F("Clear LCD"));
#endif
  clearLCD();
  delay(100);
#if (DEBUG > 0)
  serialport->println(F("Start LCD splash"));
#endif

  splashLCD();


#if (DEBUG > 0)
  serialport->println(F("EndLCD splash"));
#endif

  //Setup GPIO pins, Mute and lights
  pinMode(SWITCH_MUTE, INPUT_PULLUP);     //The SWITCH_MUTE is different on Atmega vs ESP32.  Is this redundant?
  pinMode(SWITCH_ENCODER, INPUT_PULLUP);  //The SWITCH_ENCODER is new to Krake. Is this redundant?

  for (int i = 0; i < NUM_LIGHTS; i++) {
#if (DEBUG > 0)
    serialport->print(LIGHT[i]);
    serialport->print(", ");
#endif
    pinMode(LIGHT[i], OUTPUT);
  }
  serialport->println("");

  muteButton.set(SWITCH_MUTE, muteButtonCallback);
  muteButton.enableLongPress(longPressTime);
  muteButton.enableMultiHit(multiHitTime, multiHitTarget);

  //SW4.set(GPIO_SW4, SendEmergMessage, INT_PULL_UP);
  //  encoderSwitchButton.set(SWITCH_ENCODER, encoderSwitchCallback, INT_PULL_UP);
  encoderSwitchButton.set(SWITCH_ENCODER, encoderSwitchCallback);
  encoderSwitchButton.enableLongPress(longPressTime);
  encoderSwitchButton.enableMultiHit(multiHitTime, multiHitTarget);

  printInstructions(serialport);
  AlarmMessageBuffer[0] = '\0';

  // digitalWrite(LED_BUILTIN, LOW);   // turn the LED off at end of setup

#if !defined(HMWK)  //On Homework2, LCD goes blank early
  // Here initialize the UART1
  //FLE the Serial1 is faliing to terminate
  //   pinMode(RXD1, INPUT_PULLUP);
  uartSerial1.begin(UART1_BAUD_RATE, SERIAL_8N1, RXD1, TXD1);  // UART setup. On Homework2, LCD goes blank early
  uartSerial1.flush();                                         //Clear any Serial1 crud at reset.

#if (DEBUG > 0)
  serialport->println(F("uartSerial1 Setup"));
#endif
#endif

  // Here initialize the UART2
  pinMode(RXD2, INPUT_PULLUP);
  uartSerial2.begin(UART2_BAUD_RATE, SERIAL_8N1, RXD2, TXD2);  // UART setup
  uartSerial2.flush();
#if (DEBUG > 0)
  serialport->println(F("uartSerial2 Setup"));
#endif
}  // end GPAD_HAL_setup()

// This routine should be refactored so that it only "interprets"
// the character buffer and returns an "abstract" command to be acted on
// elseshere. This will allow us to remove the PubSubClient from the this file,
// the Hardware Abstraction Layer.
void interpretBuffer(char *buf, int rlen, Stream *serialport, PubSubClient *client) {
  if (rlen < 1) {
    printError(serialport);
    return;  // no action
  }

  bool found = false;
  for (int i = 0; i < NUM_PREFICES; i++) {
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
    case 'h':  // help
      printInstructions(serialport);
      break;
    case 'a':
      {
        // In the case of an alarm state, the rest of the buffer is a message.
        // we will read up to 60 characters from this buffer for display on our
        // Arguably when we support mulitple states this will become more complicated.
        char D = buf[1];
        int N = D - '0';
        serialport->println(N);
        // WARNING: Shouldn't this be MAX_BUFFER_SIZE?
        char msg[61];
        msg[0] = '\0';
        strncat(msg, buf, 60);
        // This copy loooks uncessary, but is not...we want "alarm"
        // to be a completely independent and abstract function.
        // it should copy the msg buffer
        serialport->print("The MQTT Alarm Message: ");
        serialport->println(msg);
        alarm((AlarmLevel)N, msg, serialport);  //Makes Lamps indicate alarm.

        break;
      }
    case 'i':  //Information. Firmware Version, Mute Status,
      {
        //Firmware Version
        // 81+23 = Maximum string length
        //        char onInfoMsg[32] = "Firmware Version: ";
        //        static char onInfoMsg[81+24] = "Firmware Version: "; //This does not have the bug.
        char onInfoMsg[81 + 24] = "Firmware Version: ";  //This
        char str[20];

        strcat(onInfoMsg, FIRMWARE_VERSION);
        client->publish(publish_Ack_Topic, onInfoMsg);
        serialport->println(onInfoMsg);

        //Up time
        onInfoMsg[0] = '\0';

        str[0] = '\0';
        strcat(onInfoMsg, "System up time (mills): ");
        sprintf(str, "%d", millis());
        strcat(onInfoMsg, str);
        client->publish(publish_Ack_Topic, onInfoMsg);
        serialport->println(onInfoMsg);

        // Mute status
        onInfoMsg[0] = '\0';
        //onInfoMsg[32] = "Mute Status: ";
        strcat(onInfoMsg, "Mute Status: ");
        if (currentlyMuted) {
          strcat(onInfoMsg, "MUTED");
        } else {
          strcat(onInfoMsg, "NOT MUTED");
        }
        client->publish(publish_Ack_Topic, onInfoMsg);
        serialport->println(onInfoMsg);

        //Alarm level
        onInfoMsg[0] = '\0';
        str[0] = '\0';
        strcat(onInfoMsg, "Current alarm Level: ");
        sprintf(str, "%d", getCurrentAlarmLevel());
        strcat(onInfoMsg, str);
        client->publish(publish_Ack_Topic, onInfoMsg);
        serialport->println(onInfoMsg);

        //Alarm message
        onInfoMsg[0] = '\0';
        strcat(onInfoMsg, "Current alarm message: ");
        //        strcat(onInfoMsg, *getCurrentMessage());  Produced error error: invalid conversion from 'char' to 'const char*' [-fpermissive]
        strcat(onInfoMsg, getCurrentMessage());
        client->publish(publish_Ack_Topic, onInfoMsg);
        serialport->println(onInfoMsg);

        //IP Address
        //Serial.println(WiFi.localIP()); 

        onInfoMsg[0] = '\0';
        strcat(onInfoMsg, "IP Address: ");
        // //strcat(onInfoMsg, myIP.toString());  //This returns Compilation error: request for member 'toString' in 'myIP', which is of non-class type 'IPAddress()'

        char ipString[] = "(0,0,0,0)";        
        // // sprintf(ipString, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
        // sprintf(ipString, "%d.%d.%d.%d",  myIP[0], myIP[1], myIP[2], myIP[3]);

        strcat(onInfoMsg, ipString);  //This returns Compilation error: request for member 'toString' in 'myIP', which is of non-class type 'IPAddress()'

        client->publish(publish_Ack_Topic, onInfoMsg);
        serialport->println(onInfoMsg);

        // serialport->print("myIP =");
        // serialport->println(myIP);   // Caused Error Multiple libraries were found for "WiFiManager.h"
        
        break; //end of 'i'
      }
    default:
      serialport->println(F("Unknown Command"));
      break;
  }
  serialport->print(F("currentlyMuted : "));
  serialport->println(currentlyMuted);
  serialport->println(F("interpret Done"));
  //FLE  delay(3000);
}  //end interpretBuffer()


// This has to be called periodically, at a minimum to handle the mute_button
void GPAD_HAL_loop() {
  muteButton.poll();
  encoderSwitchButton.poll();
#if defined(GPAD)  //FLE??? Why is this conditional compile?
  muteButton.poll();
#endif
}

/* Assumes LCD has been initilized
   Turns off Back Light
   Clears display
   Turns on back light.
*/
void clearLCD(void) {
  lcd.noBacklight();
  lcd.clear();
}

//Splash a message so we can tell the LCD is working
void splashLCD(void) {
  lcd.init();  // initialize the lcd
  // Print a message to the LCD.
#if (!LIMIT_POWER_DRAW)
  lcd.backlight();
#else 
  lcd.noBacklight();
#endif

  //Line 0
  lcd.setCursor(0, 0);
  lcd.print(MODEL_NAME);
  // lcd.print(DEVICE_UNDER_TEST);
  //  lcd.setCursor(3, 1);

  //Line 1
  lcd.setCursor(0, 1);
  lcd.print(PROG_NAME);
  lcd.print(" ");
  lcd.print(FIRMWARE_VERSION);

  //Line 2
  lcd.setCursor(0, 2);
  lcd.print(F(__DATE__ " " __TIME__));

  //Line 3
  lcd.setCursor(0, 3);
  lcd.print("MAC: ");
  lcd.print(macAddressString);
}
bool printable(char c) {
  return isPrintable(c) || (c == ' ');
}
// Remove unwanted characters....
void filter_control_chars(char *msg) {
  size_t len = strlen(msg);
  char buff[MAX_BUFFER_SIZE];
  strcpy(buff, msg);
  int k = 0;
  for (int i = 0; i < len; i++) {
    char c = buff[i];
    if (printable(c)) {
      msg[k] = c;
      k++;
    }
  }
  msg[k] = '\0';
}
// TODO: We need to break the message up into strings to render properly
// on the display
void showStatusLCD(AlarmLevel level, bool muted, char *msg) {
  lcd.init();
  lcd.clear();
  // Possibly we don't need the backlight if the level is zero!
  if (level != 0) {
#if (!LIMIT_POWER_DRAW)
    lcd.backlight();
#endif
  } else {
    lcd.noBacklight();
  }

  lcd.print("LVL: ");
  lcd.print(level);
  lcd.print(" - ");
  lcd.print(AlarmNames[level]);

  int msgLineStart = 1;
  lcd.setCursor(0, msgLineStart);
  int len = strlen(AlarmMessageBuffer);
  if (len < 9) {
    if (muted) {
      lcd.print("MUTED! MSG:");
    } else {
      lcd.print("MSG:  ");
    }
    msgLineStart = 2;
  }
  if (strlen(AlarmMessageBuffer) == 0) {
    lcd.print("None.");
  } else {

    char buffer[21] = { 0 };  // note space for terminator
                              // filter unmeaningful characters from msg buffer
    filter_control_chars(msg);

    size_t len = strlen(msg);          // doesn't count terminator
    size_t blen = sizeof(buffer) - 1;  // doesn't count terminator
    size_t i = 0;
    // the actual loop that enumerates your buffer
    for (i = 0; i < (len / blen + 1) && i + msgLineStart < 4; ++i) {
      memcpy(buffer, msg + (i * blen), blen);
      local_ptr_to_serial->println(buffer);
      lcd.setCursor(0, i + msgLineStart);
      lcd.print(buffer);
    }
  }
}

// This operation is idempotent if there is no change in the abstract state.
void set_light_level(int lvl) {
  for (int i = 0; i < lvl; i++) {
    digitalWrite(LIGHT[i], HIGH);
  }
  for (int i = lvl; i < NUM_LIGHTS; i++) {
    digitalWrite(LIGHT[i], LOW);
  }
}
void unchanged_anunicateAlarmLevel(Stream *serialport) {
  unsigned long m = millis();
  unsigned long time_in_song = m - start_of_song;
  unsigned char note = time_in_song / (unsigned long)LEN_OF_NOTE_MS;
  //   serialport->print("note: ");
  //   serialport->println(note);
  if (note >= NUM_NOTES) {
    note = 0;
    start_of_song = m;
  }
  unsigned char light_lvl = LIGHT_LEVEL[currentLevel][note];
  set_light_level(light_lvl);
  // TODO: Change this to our device types
//#if !defined(HMWK)
#if defined(GPAD)
  if (!currentlyMuted) {
    unsigned char note_lvl = SONGS[currentLevel][note];

    //   serialport->print("note lvl");
    //   serialport->println(note_lvl);
    tone(TONE_PIN, BUZZER_LVL_FREQ_HZ[note_lvl], INF_DURATION);

  } else {
    noTone(TONE_PIN);
  }
#endif

}
void annunciateAlarmLevel(Stream *serialport) {
  start_of_song = millis();
  unchanged_anunicateAlarmLevel(serialport);
  showStatusLCD(currentLevel, currentlyMuted, AlarmMessageBuffer);
  // here is the new call
  serialport->println("dfPlayer.play");
  serialport->println(currentLevel);
  if (!currentlyMuted) {
    playNotBusyLevel(currentLevel);
  }
}
