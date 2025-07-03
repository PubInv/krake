#define PROG_NAME "KRAKE_wOTA_SPI_Peripheral"
// Author: Nagham Kheir
// Date: 20250316
// A PMD BootUp, Similar to Do Nothing but Serial Splash, LCD Test, LCD Splash, DFPlayer Test, DFPlayer Splash, BootButton Test for stuck key. Loop() has only non blocking code.
// https://github.com/PubInv/krake/issues/158
// Hardware: Homework2 20240421

#include <WiFiManager.h>  // WiFi Manager for ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <ElegantOTA.h>
#include <FS.h>    // File System Support
#include <Wire.h>  // req for i2c comm
#include <DFRobotDFPlayerMini.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DailyStruggleButton.h>

// Customized this by changing these defines
#define VERSION " V0.0.3 "  //Comment out
#define MODEL_NAME "KRAKE"
#define DEVICE_UNDER_TEST "LB0005"  //A Serial Number
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "LB"
#define BAUDRATE 115200  //Serial port


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notifyClients(const String &message) {
  ws.textAll(message);
}

// Some PMD Hardware

// Pins for switches and LEDs and more
#define BOOT_BUTTON 0
const int LED_BUILTIN = 2;
const int LED_1 = 15;
const int LED_2 = 4;
const int LED_3 = 5;
const int LED_4 = 18;
const int LED_5 = 19;
const int WiFiLed = 23;  // Built-in LED on ESP32

const int LED_PINS[] = { LED_1, LED_2, LED_3, LED_4, LED_5 };
// const int SWITCH_PINS[] = { SW1, SW2, SW3, SW4 };  // SW1, SW2, SW3, SW4
const int LED_COUNT = sizeof(LED_PINS) / sizeof(LED_PINS[0]);
// const int SWITCH_COUNT = sizeof(SWITCH_PINS) / sizeof(SWITCH_PINS[0]);
 

//Claases defined below
class Flasher {
  // Class Member Variables
  // These are initialized at startup
  int ledPin;    // the number of the LED pin
  long OnTime;   // milliseconds of on-time
  long OffTime;  // milliseconds of off-time

  // These maintain the current state
  int ledState;                  // ledState used to set the LED
  unsigned long previousMillis;  // will store last time LED was updated

  // Constructor - creates a Flasher
  // and initializes the member variables and state
public:
  Flasher(int pin, long on, long off) {
    ledPin = pin;
    pinMode(LED_BUILTIN, OUTPUT);

    OnTime = on;
    OffTime = off;

    ledState = LOW;
    previousMillis = 0;
  }


  void Update() {  // defining a member function of the Flasher class
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();

    if ((ledState == HIGH) && (currentMillis - previousMillis >= OnTime)) {
      ledState = LOW;                   // Turn it off
      previousMillis = currentMillis;   // Remember the time
      digitalWrite(WiFiLed, ledState);  // Update the actual LED
    } else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime)) {
      ledState = HIGH;                  // turn it on
      previousMillis = currentMillis;   // Remember the time
      digitalWrite(WiFiLed, ledState);  // Update the actual LED
    }
  }
};


void splashserial(void) {
  //Serial splash
  Serial.println(F("==================================="));
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.println(MODEL_NAME);
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(F("Compiled at: "));
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier
  Serial.println(LICENSE);
  Serial.println(F("==================================="));
  Serial.println();
}

void setup() {

  //setting GPIO to initial values
  pinMode(WiFiLed, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);     // set the LED pin mode
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  pinMode(LED_1, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_1, HIGH);        // turn the LED on (HIGH is the voltage level)
  pinMode(LED_2, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_2, HIGH);        // turn the LED on (HIGH is the voltage level)
  pinMode(LED_3, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_3, HIGH);        // turn the LED on (HIGH is the voltage level)
  pinMode(LED_4, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_4, HIGH);        // turn the LED on (HIGH is the voltage level)
  pinMode(LED_5, OUTPUT);           // set the LED pin mode
  digitalWrite(LED_5, HIGH);        // turn the LED on (HIGH is the voltage level)

  Serial.begin(BAUDRATE);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB
  }
  WiFiMan();
  // blink();
  splashserial();
  Wire.begin();
  SPIsetup();
  LCD_setup();
  // More setup code here

  digitalWrite(LED_1, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_2, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_3, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_4, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_5, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_BUILTIN, LOW);  //Make built in LED low at end of setup.

  //Elegant OTA Setup

  initWiFi();
  initLittleFS();


  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

  server.serveStatic("/", LittleFS, "/");

  // Route to control LEDs using parameters
  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("led") && request->hasParam("state")) {
      int ledIndex = request->getParam("led")->value().toInt();
      String state = request->getParam("state")->value();

      if (ledIndex >= 1 && ledIndex <= LED_COUNT) {
        digitalWrite(LED_PINS[ledIndex - 1], (state == "on") ? HIGH : LOW);
        request->send(200, "text/plain", "OK");
      } else {
        request->send(400, "text/plain", "Invalid LED index");
      }
    } else {
      request->send(400, "text/plain", "Missing parameters");
    }
  });

  // Start server
  server.begin();
  ElegantOTA.begin(&server);  // Start ElegantOTA
}
//end setup()

void loop() {

  // switchISR();
  wink();  // Heart beat aka activity indicator LED function.
  SPIloop();
  LCD_Splash();
}  //end loop()
