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

#include "WiFiManagerOTA.h"
#include "Button.h"
#include "DFP.h"
#include "OLEDDisplay.h"
#include "Wink.h"

#define BAUDRATE 115200  //Serial port \
                         // Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notifyClients(const String &message) {
  ws.textAll(message);
}

// Some PMD Hardware

// Pins for switches and LEDs and more / HMWK2
#define BOOT_BUTTON 0
const int LED_BUILTIN = 2;
const int LED_1 = 15;
const int LED_2 = 4;
const int LED_3 = 5;
const int LED_4 = 18;
const int LED_5 = 19;
int WiFiLed = 23;  // Built-in LED on ESP32


// Pins for switches and LEDs and more //Krake
// #define BOOT_BUTTON 0
// const int LED_BUILTIN = 2;
// const int LED_1 = 12;
// const int LED_2 = 14;
// const int LED_3 = 27;
// const int LED_4 = 26;
// const int LED_5 = 25;
// int WiFiLed = 23;  // Built-in LED on ESP32
// add and define SWITCH_MUTE 35

const int LED_PINS[] = { LED_1, LED_2, LED_3, LED_4, LED_5 };
// const int SWITCH_PINS[] = { SW1, SW2, SW3, SW4 };  // SW1, SW2, SW3, SW4
const int LED_COUNT = sizeof(LED_PINS) / sizeof(LED_PINS[0]);
// const int SWITCH_COUNT = sizeof(SWITCH_PINS) / sizeof(SWITCH_PINS[0]);

// bool dfpAvailable = false;
bool availableDFPLAYER = false;

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

//Defining the behavior
// Flasher led1(ledpin#, on time, off time);
Flasher led1(LED_1, 250, 250);                          // LED 1 blinks 0.5 period and 50% duty cycle
Flasher led2(LED_2, 1200 * 20 / 100, 1200 * 80 / 100);  // LED 2 blinks 1.2 period and 20% duty cycle
Flasher led3(LED_3, 1400 * 62 / 100, 1400 * 38 / 100);  // LED 3 blinks 1.4 period and 62% duty cycle
Flasher led4(LED_4, 3000 * 55 / 100, 3000 * 45 / 100);  // LED 4 blinks 3.0 period and 55% duty cycle
Flasher led5(LED_5, 7500 * 10 / 100, 7500 * 90 / 100);  // LED 5 blinks 7.5 period and 10% duty cycle


//Elegant OTA Setup

void setupOTA() {

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

  // End of ELegant OTA Setup
}


void setup() {

  //setting GPIO to initial values
  pinMode(WiFiLed, OUTPUT);         // set the LED pin mode
  digitalWrite(WiFiLed, HIGH);      // turn the LED on (HIGH is the voltage level)
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

  splashserial();
  Wire.begin();
  initOLED();
  splashOLED();
  initDFP();
  splashDFPlayer();
  setupButton();
  WiFiMan();
  initWiFi();
  initLittleFS();
  setupOTA();
  server.begin();             // Start server
  ElegantOTA.begin(&server);  // Start ElegantOTA
  // More setup code here
  digitalWrite(LED_1, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_2, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_3, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_4, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_5, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_BUILTIN, LOW);  //Make built in LED low at end of setup.
}  //end setup()

void loop() {
  wink();  // Heart beat aka activity indicator LED function.
  led1.Update();
  led2.Update();
  led3.Update();
  led4.Update();
  led5.Update();
  // This is needed to poll the button constantly
  myButton.poll();
  SW1.poll();
  SW2.poll();
  SW3.poll();
  SW4.poll();
}  //end loop()
