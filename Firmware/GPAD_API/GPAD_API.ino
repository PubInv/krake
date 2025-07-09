/* GPAD_API.ino
  The program implements the main API of the General Purpose Alarm Device.

  Copyright (C) 2025 Robert Read

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
#include "GPAD_HAL.h"
#include "gpad_utility.h"
#include "gpad_serial.h"
#include "wink.h"
#include <Math.h>

#include <WiFi.h>
#include <esp_wifi.h>

#include <PubSubClient.h>  // From library https://github.com/knolleary/

#include <WiFiManager.h>  // WiFi Manager for ESP32
#include "LittleFS.h"
#include <ElegantOTA.h>
#include <FS.h>    // File System Support
#include <Wire.h>  // req for i2c comm

#include "WiFiManagerOTA.h"
#include <ESPAsyncWebServer.h>

#include "InterruptRotator.h"

#include "DFPlayer.h"
#include "GPAD_menu.h"


AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Initialize WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);


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

//#define DEBUG 0
#define DEBUG 1
//#define DEBUG 4

unsigned long last_command_ms;

// We have currently defined our alam time to include 10-second "songs",
// So we will not process a new alarm condition until we have completed one song.
const unsigned long DELAY_BEFORE_NEW_COMMAND_ALLOWED = 10000;
const unsigned int NUM_WIFI_RECONNECT_RETRIES = 3;

const int LED_PINS[] = { LIGHT0, LIGHT1, LIGHT2, LIGHT3, LIGHT4 };
// const int SWITCH_PINS[] = { SW1, SW2, SW3, SW4 };  // SW1, SW2, SW3, SW4
const int LED_COUNT = sizeof(LED_PINS) / sizeof(LED_PINS[0]);
// const int SWITCH_COUNT = sizeof(SWITCH_PINS) / sizeof(SWITCH_PINS[0]);

//Aley network
// const char* ssid = "Home";
// const char* password = "adt@1963#";

//Maryville network
// const char* ssid = "VRX";
// const char* password = "textinsert";

//Houstin network
// const char* ssid = "DOS_WIFI";
// const char* password = "$Suve07$$";

// Austin network
const char* ssid = "readfamilynetwork";
const char* password = "magicalsparrow96";


// MQTT Broker
const char* mqtt_server = "public.cloud.shiftr.io";
const char* mqtt_user = "public";
const char* mqtt_password = "public";

// MQTT Topics, MAC plus an extention
// A MAC addresss treated as a string has 12 chars.
// The strings "_ALM" and "_ACK" have 4 chars.
// A null character is one other. 12 + 4 + 1 = 17
char subscribe_Alarm_Topic[17];
char publish_Ack_Topic[17];
char macAddressString[13];

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MACSTR_PLN "%02X%02X%02X%02X%02X%02X"

// // Initialize WiFi and MQTT clients
// WiFiClient espClient;
// PubSubClient client(espClient);

// String myMAC = "";

// #define VERSION 0.02             //Version of this software
#define BAUDRATE 115200
// #define BAUDRATE 57600
// Let's have a 10 minute time out to allow people to compose strings by hand, but not
// to leave data our there forever
// #define SERIAL_TIMEOUT_MS 600000
#define SERIAL_TIMEOUT_MS 1000

//Set LED wink parameters
const int HIGH_TIME_LED_MS = 800;  //time in milliseconds
const int LOW_TIME_LED_MS = 200;
unsigned long lastLEDtime_ms = 0;
// unsigned long nextLEDchangee_ms = 100; //time in ms.
unsigned long nextLEDchangee_ms = 5000;  //time in ms.

// extern int LIGHT[];
// extern int NUM_LIGHTS;

void serialSplash() {
  //Serial splash
  Serial.println(F("==================================="));
  Serial.println(COMPANY_NAME);
  Serial.println(MODEL_NAME);
  //  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(PROG_NAME);
  Serial.println(FIRMWARE_VERSION);
  //  Serial.println(HARDWARE_VERSION);
  Serial.print("Builtin ESP32 MAC Address: ");
  Serial.println(macAddressString);
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
  const unsigned long MESSAGE_PERIOD = 10000;
  static unsigned long lastMillis = 0;  // Sets timing for periodic MQTT publish message
  // publish a message roughly every second.
  if ((millis() - lastMillis > MESSAGE_PERIOD) || (millis() < lastMillis)) {  //Check for role over.
    lastMillis = lastMillis + MESSAGE_PERIOD;

    float rssi = WiFi.RSSI();
    char rssiString[8];

#if (DEBUG > 1)
    Serial.print("Publish RSSI: ");
    Serial.println(rssi);
#endif

    dtostrf(rssi, 1, 2, rssiString);
    char onLineMsg[32] = " online, RSSI:";
    strcat(onLineMsg, rssiString);
    client.publish(publish_Ack_Topic, onLineMsg);

    // This should be moved to a place after the WiFi connect success
    //  Serial.print("Device connected at IPaddress: "); //FLE
    // Serial.println(WiFi.localIP());  //FLE

#if defined(HMWK)
    digitalWrite(LED_D9, !digitalRead(LED_D9));  // Toggle
#endif
  }
}



bool connect_to_wifi() {
  if (WiFi.status() != WL_CONNECTED) {
    delay(10);
    Serial.println();

    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Failed to connect WiFi.");
      return false;
    } else {
      Serial.print("WiFi connected");

#if (DEBUG > 1)
      delay(100);
      Serial.print("Device connected at IPaddress: ");  //FLE
      Serial.println(WiFi.localIP());                   //FLE
#endif
      return true;
    }
  }
  return true;
}  // end connect_to_wifi()

// TODO: have this return a success or failure status and move
// the delay up.
void reconnect() {
  int n = 0;
  while (!client.connected() && n < NUM_WIFI_RECONNECT_RETRIES) {
    n++;
    Serial.print("Attempting MQTT connection...");
    if (client.connect(COMPANY_NAME, mqtt_user, mqtt_password)) {
      Serial.println("success!");
      client.subscribe(subscribe_Alarm_Topic);  // Subscribe to GPAD API alarms
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(1000);
    }
  }
  Serial.println((client.connected()) ? "connected!" : "failed to reconnect!");
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



// Handeler for MQTT subscribed messages
void callback(char* topic, byte* payload, unsigned int length) {
  // todo, remove use of String here....
  // Note: We will check for topic or topics in the future...
  if (strcmp(topic, subscribe_Alarm_Topic) == 0) {
    char mbuff[121];
    Serial.print("Topic arrived [");
    Serial.print(topic);
    Serial.print("] ");

    //Put payload into mbuff[] a character array
    int m = min((unsigned int)length, (unsigned int)120);
    for (int i = 0; i < m; i++) {
      mbuff[i] = (char)payload[i];
    }
    mbuff[m] = '\0';

#if (DEBUG > 0)
    Serial.print("|");
    Serial.print(mbuff);
    Serial.println("|");
#endif

    Serial.println("Received MQTT Msg.");
    interpretBuffer(mbuff, m, &Serial, &client);  //Process the MQTT message
    annunciateAlarmLevel(&Serial);
  }
}  //end call back

bool readMacAddress(uint8_t* baseMac) {
  //  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    // Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
    //               baseMac[0], baseMac[1], baseMac[2],
    //               baseMac[3], baseMac[4], baseMac[5]);
    return true;
  } else {
    // Serial.println("Failed to read MAC address");
    return false;
  }
}

//Elegant OTA Setup

void setupOTA() {

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

  server.serveStatic("/", LittleFS, "/");


  // End of ELegant OTA Setup
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // set the LED pin mode
  digitalWrite(LED_BUILTIN, HIGH);
  //Serial setup
  delay(100);
  Serial.begin(BAUDRATE);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB
  }
  serialSplash();
  // We call this a second time to get the MAC on the screen
  clearLCD();
  splashLCD();

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

  //Init arrays.
  subscribe_Alarm_Topic[0] = '\0';
  publish_Ack_Topic[0] = '\0';
  macAddressString[0] = '\0';

#if (DEBUG > 1)
  Serial.println("Call: GPAD_HAL_setup(&Serial)");
#endif

  //Setup and present LCD splash screen
  //Setup the SWITCH_MUTE
  //Setup the SWITCH_ENCODER
  GPAD_HAL_setup(&Serial);

#if (DEBUG > 0)
  Serial.println("MAC: ");
  Serial.println(macAddressString);
#endif

  Serial.setTimeout(SERIAL_TIMEOUT_MS);
  client.setServer(mqtt_server, 1883);  //Default MQTT port
  client.setCallback(callback);

#if (DEBUG > 0)
  Serial.println("Starting WiFi as STA");
#endif


  // Note: On Krake SN#3 only, performing this
  // while the Splash is on causes a reset, presumably
  // because too much power is drawn. I am using a conditional
  // to isolate this as much as possible, while
  // still allowing us to use a single code base for all hardware
  // devices -- rlr

#if (LIMIT_POWER_DRAW)
  clearLCD();
#endif

  WiFi.mode(WIFI_STA);
  WiFi.STA.begin();

#if (LIMIT_POWER_DRAW)
  splashLCD();
#endif

  // setup_spi();

  uint8_t mac[6];
  readMacAddress(mac);
  char buff[13];
  sprintf(buff, MACSTR_PLN, MAC2STR(mac));

#if (DEBUG > 0)
  printf("My mac is " MACSTR "\n", MAC2STR(mac));
  Serial.print("MAC as char array: ");
  Serial.println(buff);
#endif

  strcpy(macAddressString, buff);
  macAddressString[12] = '\0';
  strcpy(subscribe_Alarm_Topic, buff);
  strcpy(publish_Ack_Topic, buff);
  strcpy(subscribe_Alarm_Topic + 12, "_ALM");
  strcpy(publish_Ack_Topic + 12, "_ACK");
  subscribe_Alarm_Topic[16] = '\0';
  publish_Ack_Topic[16] = '\0';

#if (DEBUG > 1)
  Serial.println("XXXXXXX");
  Serial.println(subscribe_Alarm_Topic);
  Serial.println(publish_Ack_Topic);
  Serial.println("XXXXXXX");
#endif

  // We call this a second time to get the MAC on the screen
  //  clearLCD();
  // req for Wifi Man and OTA
  WiFiMan();
  initLittleFS();
  server.begin();  // Start server web socket to render pages
  ElegantOTA.begin(&server);
  setupOTA();

  // Need this to work here:   printInstructions(serialport);
  Serial.println(F("Done With Setup!"));
  turnOnAllLamps();
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off at end of setup

  initRotator();
  splashLCD();


 setupDFPlayer();
 setup_GPAD_menu();

}  // end of setup()

unsigned long last_ms = 0;
void toggle(int pin) {
  digitalWrite(pin, digitalRead(pin) ? LOW : HIGH);
}

const unsigned long LOW_FREQ_DEBUG_MS = 20000;
unsigned long time_since_LOW_FREQ_ms = 0;

//IPAddress myIP(0, 0, 0, 0); // declare for global and initialize
IPAddress myIP();  // declare for global

int cnt_actions = 0;

bool running_menu = false;
bool menu_just_exited = false;

void loop() {

  bool is_WIFIconnected = false;
  unsigned long ms = millis();
  if (ms - time_since_LOW_FREQ_ms > LOW_FREQ_DEBUG_MS) {
    time_since_LOW_FREQ_ms = ms;

    //If WiFi was not connected and becomes connected then print IP address
    if (!is_WIFIconnected && connect_to_wifi()) {
      is_WIFIconnected = true;

      //Get the IP address into a variable I can make global
      IPAddress myIP = WiFi.localIP();
      const char* ipString = myIP.toString().c_str();
      // strcat(onInfoMsg, *getCurrentMessage());  Produced error error: invalid conversion from 'char' to 'const char*' [-fpermissive]

      Serial.print("Device connected at IPaddress: ");  //FLE
                                                        //      Serial.println(WiFi.localIP());                   //FLE
      Serial.println(myIP);                             //FLE
    }


    // Serial.println(subscribe_Alarm_Topic);
    // Serial.println(publish_Ack_Topic);
#if defined HMWK || defined KRAKE
    if (!client.connected()) {
      reconnect();
    }
#endif
  }


#if defined HMWK || defined KRAKE
  client.loop();
  publishOnLineMsg();
  wink();  //The builtin LED
#endif

  unchanged_anunicateAlarmLevel(&Serial);
  // delay(20);
  GPAD_HAL_loop();

  processSerial(&Serial, &Serial, &client);

  // Here we also process the UART1 using the same routine.
  processSerial(&Serial, &uartSerial1, &client);

  // Here we will listen for an SPI command...

  // // Now try to read from the SPI Port!
#if defined(GPAD)
  updateFromSPI();
#endif

  updateRotator();

  if (menu_just_exited) {
    lcd.clear();
    lcd.noBacklight();
    restoreAlarmLevel(&Serial);
    menu_just_exited = false;
  }
  if (running_menu) {
    lcd.backlight();
    poll_GPAD_menu();
  }

  // if ((millis() / 10000) > cnt_actions) {
  //   cnt_actions++;
  //   navigate_to_n_and_execute(cnt_actions % 3);
  // }
}
