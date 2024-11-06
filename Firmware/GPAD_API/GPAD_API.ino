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
#include <Math.h>

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




#define DEBUG_SPI 0
#define DEBUG 0


unsigned long last_command_ms;

// We have currently defined our alam time to include 10-second "songs",
// So we will not process a new alarm condition until we have completed one song.
const unsigned long DELAY_BEFORE_NEW_COMMAND_ALLOWED = 10000;



//Maryville network
 const char* ssid = "VRX";
 const char* password = "textinsert";

// Austin network
//const char* ssid = "readfamilynetwork";
//const char* password = "magicalsparrow96";


// MQTT Broker
const char* mqtt_server = "public.cloud.shiftr.io";
const char* mqtt_user = "public";
const char* mqtt_password = "public";

// MQTT Topics
// User must modify the device serial number. 
//In this case change the part "USA4" as approprate by commenting out conflicting lines.
//const char* subscribe_Alarm_Topic = "KRAKE_20240421_LEB1_ALM";
//const char* publish_Ack_Topic = "KRAKE_20240421_LEB1_ACK";

const char* subscribe_Alarm_Topic = "KRAKE_20240421_USA4_ALM";
const char* publish_Ack_Topic = "KRAKE_20240421_USA4_ACK";

//const char* subscribe_Alarm_Topic = "KRAKE_20240421_USA5_ALM";
//const char* publish_Ack_Topic = "KRAKE_20240421_USA5_ACK";

// Initialize WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);



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
  Serial.println(MODEL_NAME);
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(PROG_NAME);
  Serial.println(FIRMWARE_VERSION);
//  Serial.println(HARDWARE_VERSION);
  Serial.print("Builtin ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print(F("Alarm Topic: "));
  Serial.println(subscribe_Alarm_Topic);
  Serial.print(F("Broker: "));
  Serial.println(mqtt_server);
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
#if defined(HMWK)
    digitalWrite(LED_D9, !digitalRead(LED_D9));   // Toggle
#endif
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
#if defined(HMWK)
  digitalWrite(LED_D9, HIGH);
#endif
  digitalWrite(LIGHT0, HIGH);
  digitalWrite(LIGHT1, HIGH);
  digitalWrite(LIGHT2, HIGH);
  digitalWrite(LIGHT3, HIGH);
  digitalWrite(LIGHT4, HIGH);
}
void turnOffAllLamps() {
#if defined(HMWK)
   digitalWrite(LED_D9, LOW);
#endif
  digitalWrite(LIGHT0, LOW);
  digitalWrite(LIGHT1, LOW);
  digitalWrite(LIGHT2, LOW);
  digitalWrite(LIGHT3, LOW);
  digitalWrite(LIGHT4, LOW); 
}

char mbuff[121];

// Handeler for MQTT subscrived messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  int m = min((unsigned int) length,(unsigned int) 120);
  for (int i = 0; i < m; i++) {
    mbuff[i] = (char)payload[i];
  }
  mbuff[m] = '\0';
  Serial.print("|");
  Serial.print(mbuff);
  Serial.println("|");
// todo, remove use of String here....
  if (String(topic) ==  subscribe_Alarm_Topic) {
    Serial.println("Got MessageFromProcessing_PMD");
    interpretBuffer(mbuff,m,&Serial);
  }
}//end call back


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
  digitalWrite(LED_BUILTIN, HIGH);
  //Serial setup
  delay(100);
  Serial.begin(BAUDRATE);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  delay(500);                         //Wait before sending the first data to terminal
  Serial.setTimeout(SERIAL_TIMEOUT_MS);
  serialSplash();

  // Set LED pins as outputs
#if defined(LED_D9)
  pinMode(LED_D9, OUTPUT);
#endif
  pinMode(LIGHT0, OUTPUT);
  pinMode(LIGHT1, OUTPUT);
  pinMode(LIGHT2, OUTPUT);
  pinMode(LIGHT3, OUTPUT);
  pinMode(LIGHT4, OUTPUT);
  // Turn off all LEDs initially
  turnOnAllLamps();

  //Setup and present LCD splash screen
  robot_api_setup(&Serial);

  setup_wifi();
//  client.setServer(mqtt_server, 1883 MQTT_DEFAULT_PORT);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // setup_spi();

// Need this to work here:   printInstructions(serialport);
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off at end of setup
  Serial.println(F("Done With Setup!"));
}// end of setup()

unsigned long last_ms = 0;
void toggle(int pin) {
    digitalWrite(pin, digitalRead(pin) ? LOW : HIGH); 
}
void loop() {

#if defined(HMWK)
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  publishOnLineMsg();
  wink(); //The builtin LED
#endif

  unchanged_anunicateAlarmLevel(&Serial);
 // delay(20);
 // This causes this the HMWK device to rail..
 #if defined(GPAD)
  robot_api_loop();
 #endif

  processSerial(&Serial);

  // // Now try to read from the SPI Port!
#if defined(GPAD)
  updateFromSPI();
#endif

  if (DEBUG > 1) {
    unsigned long ms = millis();
    if ((ms - last_ms) > 3000) {
      Serial.print(" LED : ");
      Serial.println(LED_BUILTIN);
      last_ms = ms;
    }
  }
}
