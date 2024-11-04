 /* GPAD_API.ino
  The program implements the main API of the General Purpose Alarm Device.

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

  Change notes:
  20221024 Creation by Rob.
  20221028 Report Program Name in splash screen. Lee Erickson
*/

/* This is a work-in-progress but it has two purposes.
 * It essentially implements two APIs: An "abstract" API that is
 * intended to be unchanging and possibly implemented on a large
 * variety of hardware devices. That is, as the GPAD hardware
 * changes and evolves, it does not invalidate the use of this API.
 *
 * Secondly, it offers a fully robotic API; that is, it gives
 * complete access to all of the hardware currently on the board.
 * For example, the current hardware, labeled Prototype #1, offers
 * a simple "tone" buzzer. The abstract interface uses this as part
 * of an abstract command like "set alarm level to PANIC".
 * The robotic control allows you to specify the actual tone to be played.
 * The robotic inteface obviously changes as the hardware changes.
 *
 * Both APIs are useful in different situations. The abstract interface
 * is good for a medical device manufacturer that expects the alarming
 * device to change and evolve. The Robotic API is good for testing the
 * actual hardware, and for a hobbyist that wants to use the device for
 * more than simple alarms, such as for implementing a game.
 *
 * It is our intention that the API will be available both through the
 * serial port and through an SPI interface. Again, these two modes
 * serve different purposes. The SPI interface is good for tight
 * intergration with a safey critical devices. The serial port approach
 * is easier for testing and for a hobbyist to develop an approach,
 * whether they eventually intend to use the SPI interface or not.
 * -- rlr, Oct. 24, 2022
 */

#include "alarm_api.h"
#include "robot_api.h"
#include "gpad_utility.h"
#include "gpad_serial.h"
#include "wink.h"

#include <WiFi.h>
#include <PubSubClient.h> // From library https://github.com/knolleary/pubsubclient


/* SPI_PERIPHERAL
   From: https://circuitdigest.com/microcontroller-projects/arduino-spi-communication-tutorial
   Modified by Forrest Lee Erickson 20220523
   Change to Controller/Peripheral termonology
   Change variable names for start with lowercase. Constants to upper case.
   Peripherial Arduino Code:
   License: Dedicated to the Public Domain
   Warrenty: This program is designed to kill and render the earth uninhabitable,
   however it is not guaranteed to do so.
   20220524 Get working with the SPI_CONTROLER sketch. Made function updateFromSPI().
   20220925 Changes for GPAD Version 1 PCB.  SS on pin 7 and LED_PIN on D3.
   20220927 Change back for GPAD nCS on Pin 10
*/

//SPI PERIPHERAL (ARDUINO UNO)
//SPI COMMUNICATION BETWEEN TWO ARDUINO UNOs
//CIRCUIT DIGEST

/* Hardware Notes Peripheral
   SPI Line Pin in Arduino, IO setup
  MOSI 11 or ICSP-4  Input
  MISO 12 or ICSP-1 Output
  SCK 13 or ICSP-3  Input
  SS 10 Input
*/

#define GPAD_VERSION1

#ifdef GPAD_VERSION1 //The Version 1 PCB.
//#define SS 7                                // nCS aka /SS Input on GPAD Version 1 PCB.

#if defined(ESP32)
// const int LED_D9 = 23;  // Mute1 LED on PMD
#define LED_PIN 23                         // for GPAD LIGHT0
#define BUTTON_PIN 2                      //GPAD Button to GND,  10K Resistor to +5V.
#else // compile for an UNO, for example...
#define LED_PIN PD3                         // for GPAD LIGHT0
#define BUTTON_PIN PD2                      //GPAD Button to GND,  10K Resistor to +5V.
#endif

#else //The proof of concept wiring.
#define LED_PIN 7
#define BUTTON_PIN 2                          //Button to GND, 10K Resistor to +5V.
#endif


#define DEBUG_SPI 0
#define DEBUG 2

#include<SPI.h>



volatile boolean isReceived_SPI;
volatile byte peripheralReceived ;

volatile bool procNewPacket = false;
volatile byte indx = 0;
volatile boolean process;

byte received_signal_raw_bytes[MAX_BUFFER_SIZE];

unsigned long last_command_ms;

// We have currently defined our alam time to include 10-second "songs",
// So we will not process a new alarm condition until we have completed one song.
const unsigned long DELAY_BEFORE_NEW_COMMAND_ALLOWED = 10000;



//Maryville network
// const char* ssid = "VRX";
// const char* password = "textinsert";

// Austin network
const char* ssid = "readfamilynetwork";
const char* password = "magicalsparrow96";


// MQTT Broker
const char* mqtt_server = "public.cloud.shiftr.io";
const char* mqtt_user = "public";
const char* mqtt_password = "public";

// MQTT Topics
// User must modify the device serial number. In this case change the part "USA4" as approprate.
const char* subscribe_Alarm_Topic = "KRAKE_20240421_USA5_ALM";
const char* publish_Ack_Topic = "KRAKE_20240421_USA5_ACK";

// Initialize WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);


void setup_spi()
{
  Serial.println(F("Starting SPI Peripheral."));
  Serial.print(F("Pin for SS: "));
  Serial.println(SS);

  pinMode(BUTTON_PIN, INPUT);              // Setting pin 2 as INPUT
  pinMode(LED_PIN, OUTPUT);                // Setting pin 7 as OUTPUT

  //  SPI.begin();    // IMPORTANT. Do not set SPI.begin for a peripherial device.
  pinMode(SS, INPUT_PULLUP); //Sets SS as input for peripherial
  // Why is this not input?
  pinMode(MOSI, INPUT);    //This works for Peripheral
  pinMode(MISO, OUTPUT);    //try this.
  pinMode(SCK, INPUT);                  //Sets clock as input
  #if !defined(ESP32)
  SPCR |= _BV(SPE);                       //Turn on SPI in Peripheral Mode
  // turn on interrupts
  SPCR |= _BV(SPIE);

  isReceived_SPI = false;
  SPI.attachInterrupt();                  //Interuupt ON is set for SPI commnucation
  #else
  #endif
}//end setup()

//ISRs
// This is the original...
// I plan to add an index to this to handle the full message that we intend to receive.
// However, I think this also needs a timeout to handle the problem of getting out of synch.
const int SPI_BYTE_TIMEOUT_MS = 200; // we don't get the next byte this fast, we reset.
volatile unsigned long last_byte_ms = 0;

#if defined(ESP32)
// void IRAM_ATTR ISR() {
//    receive_byte(SPDR);
// }
#elif defined(ARDUINO) // compile for an UNO, for example...
ISR (SPI_STC_vect)                        //Inerrrput routine function
{
   receive_byte(SPDR);
}//end ISR
#endif



void receive_byte(byte c)
{
  last_byte_ms = millis();
  // byte c = SPDR; // read byte from SPI Data Register
  if (indx < sizeof received_signal_raw_bytes) {
    received_signal_raw_bytes[indx] = c; // save data in the next index in the array received_signal_raw_bytes
    indx = indx + 1;
  }
  if (indx >= sizeof received_signal_raw_bytes) {
    process = true;
  }
}


void updateFromSPI()
{
   if (DEBUG > 0) {
    if (process) {
      Serial.println("process true!");
    }
  }
  if(process)
  {
   
    AlarmEvent event;
    event.lvl = (AlarmLevel) received_signal_raw_bytes[0];
    for(int i = 0; i < MAX_MSG_LEN; i++) {
      event.msg[i] = (char) received_signal_raw_bytes[1+i];
    }

    if (DEBUG > 1) {
      Serial.print(F("LVL: "));
      Serial.println(event.lvl);
      Serial.println(event.msg);
    }
    int prevLevel = alarm((AlarmLevel) event.lvl, event.msg,&Serial);
    if (prevLevel != event.lvl) {
      annunciateAlarmLevel(&Serial);
    } else {
      unchanged_anunicateAlarmLevel(&Serial);
    }
  
    indx = 0;
    process = false;

  }
}

// #define VERSION 0.02             //Version of this software
#define BAUDRATE 115200
// #define BAUDRATE 57600
// Let's have a 10 minute time out to allow people to compose strings by hand, but not
// to leave data our there forever
// #define SERIAL_TIMEOUT_MS 600000
#define SERIAL_TIMEOUT_MS 1000

//Set LED wink parameters
const int HIGH_TIME_LED_MS = 800;    //time in milliseconds
const int LOW_TIME_LED_MS = 200;
unsigned long lastLEDtime_ms = 0;
// unsigned long nextLEDchangee_ms = 100; //time in ms.
unsigned long nextLEDchangee_ms = 5000; //time in ms.

// extern int LIGHT[];
// extern int NUM_LIGHTS;

void serialSplash() {
  //Serial splash
  Serial.println(F("==================================="));
  Serial.print(PROG_NAME);
  Serial.println(FIRMWARE_VERSION);
  Serial.println(HARDWARE_VERSION);
  Serial.println(MODEL_NAME);
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print("Alarm Topic: ");
  Serial.println(subscribe_Alarm_Topic);
  Serial.print(F("Compiled at: "));
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier
  Serial.println(LICENSE);
  Serial.println(F("==================================="));
  Serial.println();
}

// A periodic message identifying the subscriber (Krake) is on line.

void publishOnLineMsg(void) {
  static unsigned long lastMillis = 0;  // Sets timing for periodic MQTT publish message
  // publish a message roughly every second.
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();
    client.publish(publish_Ack_Topic, " is online");
    digitalWrite(LED_D9, !digitalRead(LED_D9));   // Toggle
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_Receiver", mqtt_user, mqtt_password)) {
      Serial.println("success!");
      client.subscribe(subscribe_Alarm_Topic);    // Subscribe to GPAD API alarms
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}
// Function to turn on all lamps
void turnOnAllLamps() {
  digitalWrite(LED_D9, HIGH);
  digitalWrite(LIGHT0, HIGH);
  digitalWrite(LIGHT1, HIGH);
  digitalWrite(LIGHT2, HIGH);
  digitalWrite(LIGHT3, HIGH);
  digitalWrite(LIGHT4, HIGH);
}
void turnOffAllLamps() {
   digitalWrite(LED_D9, LOW);
  digitalWrite(LIGHT0, LOW);
  digitalWrite(LIGHT1, LOW);
  digitalWrite(LIGHT2, LOW);
  digitalWrite(LIGHT3, LOW);
  digitalWrite(LIGHT4, LOW); 
}
// Proccess sort of like the GPAD API payload
void proccessPayloadOnLamps(String &payload) {

  if (payload.charAt(0) == 'h') {
    Serial.println("Got request for help");
    client.publish(publish_Ack_Topic, "Got request for help");
  } else if (payload.charAt(0) == 's') {
    Serial.println("Got set mute");
    client.publish(publish_Ack_Topic, "Got set mute");
  } else if (payload.charAt(0) == 'u') {
    Serial.println("Got set Unmute");
    client.publish(publish_Ack_Topic, "Got set Unmute");
  } else if (payload.charAt(0) == 'a') {
    //Parse on second character
    if ((payload.charAt(1) == '0')) {
//      client.publish(publish_Ack_Topic, "Alarm " + payload.charAt(1));
      client.publish(publish_Ack_Topic, "Alarm 0" );
      //Turn none on
      digitalWrite(LIGHT0, LOW);
      digitalWrite(LIGHT1, LOW);
      digitalWrite(LIGHT2, LOW);
      digitalWrite(LIGHT3, LOW);
      digitalWrite(LIGHT4, LOW);
    } else if ((payload.charAt(1) == '1')) {
       client.publish(publish_Ack_Topic, "Alarm 1");
      //Turn on only LAMP 1
      digitalWrite(LIGHT0, HIGH);
      digitalWrite(LIGHT1, LOW);
      digitalWrite(LIGHT2, LOW);
      digitalWrite(LIGHT3, LOW);
      digitalWrite(LIGHT4, LOW);
    } else if ((payload.charAt(1) == '2')) {
      client.publish(publish_Ack_Topic, "Alarm 2");
      //Turn on only LAMP 2
      digitalWrite(LIGHT0, LOW);
      digitalWrite(LIGHT1, HIGH);
      digitalWrite(LIGHT2, LOW);
      digitalWrite(LIGHT3, LOW);
      digitalWrite(LIGHT4, LOW);
    } else if ((payload.charAt(1) == '3')) {
      client.publish(publish_Ack_Topic, "Alarm 3");
      //Turn on only LAMP 3
      digitalWrite(LIGHT0, LOW);
      digitalWrite(LIGHT1, LOW);
      digitalWrite(LIGHT2, HIGH);
      digitalWrite(LIGHT3, LOW);
      digitalWrite(LIGHT4, LOW);
    } else if ((payload.charAt(1) == '4')) {
      client.publish(publish_Ack_Topic, "Alarm 4");
      //Turn on only LAMP 4
      digitalWrite(LIGHT0, LOW);
      digitalWrite(LIGHT1, LOW);
      digitalWrite(LIGHT2, LOW);
      digitalWrite(LIGHT3, HIGH);
      digitalWrite(LIGHT4, LOW);
    } else if ((payload.charAt(1) == '5')) {
      client.publish(publish_Ack_Topic, "Alarm 5");
      //Turn on only LAMP 5
      digitalWrite(LIGHT0, LOW);
      digitalWrite(LIGHT1, LOW);
      digitalWrite(LIGHT2, LOW);
      digitalWrite(LIGHT3, LOW);
      digitalWrite(LIGHT4, HIGH);
    } else if ((payload.charAt(1) == '6')) {
      client.publish(publish_Ack_Topic, "Alarm 6");
      //Turn on all lamps
      turnOnAllLamps();
    } else {// other than 0-6
      client.publish(publish_Ack_Topic, "Unrecognized alarm");
    }// end parsing message
  } else {
    Serial.println("Unrecognized command: " + payload.charAt(0));
    client.publish(publish_Ack_Topic, "Unrecognized command: ");
  }
}// end proccessPayloadOnLamps


// Handeler for MQTT subscrived messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (String(topic) ==  subscribe_Alarm_Topic) {
    Serial.println("Got MessageFromProcessing_PMD");
    proccessPayloadOnLamps(message);  // Change LAMPS baised on the payload
  }
}//end call back


void setup() {
  //Serial setup
  delay(100);
  Serial.begin(BAUDRATE);
  delay(100);                         //Wait before sending the first data to terminal
  Serial.setTimeout(SERIAL_TIMEOUT_MS);
  Serial.println(FIRMWARE_VERSION);

 // serialSplash();
   //Lets make the LED high near the start of setup for visual clue
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
  // Set LED pins as outputs
  pinMode(LED_D9, OUTPUT);
  pinMode(LIGHT0, OUTPUT);
  pinMode(LIGHT1, OUTPUT);
  pinMode(LIGHT2, OUTPUT);
  pinMode(LIGHT3, OUTPUT);
  pinMode(LIGHT4, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level) 

  // Turn off all LEDs initially
  turnOnAllLamps();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  robot_api_setup(&Serial);

 // setup_spi();
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off at end of setup
  Serial.println(F("Done With Setup!"));
}// end of setup()

unsigned long last_ms = 0;
void toggle(int pin) {
    digitalWrite(pin, digitalRead(pin) ? LOW : HIGH); 
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  publishOnLineMsg();
  wink(); //The builtin LED
// TODO: These need to be changed to use the "HMWK2" kind
// because we are now using "songs", we need to call this periodically
  // unchanged_anunicateAlarmLevel(&Serial);
  // delay(20);
  // robot_api_loop();

  // processSerial(&Serial);

  // // Now try to read from the SPI Port!
  // updateFromSPI();

  if (DEBUG > 1) {
    unsigned long ms = millis();
    if ((ms - last_ms) > 3000) {
      Serial.print(" LED : ");
      Serial.println(LED_BUILTIN);
      Serial.println("INDX :");
      Serial.println(indx);
      last_ms = ms;
    }
  }
}
