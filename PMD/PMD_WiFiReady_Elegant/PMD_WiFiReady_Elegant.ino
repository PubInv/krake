#define PROG_NAME "PMD_WiFiReady_Elegant"
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
#include <FS.h>  // File System Support

// Customized this by changing these defines
#define VERSION " V0.0.1 "
#define MODEL_NAME "Model: HW2_WiFiReady_Elegant"
#define DEVICE_UNDER_TEST "SN: 00001"  //A Serial Number
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "LB"

#define BAUDRATE 115200  //Serial port

#include <Wire.h>
#include <DFRobotDFPlayerMini.h>
HardwareSerial mySerial1(2);  // Use UART2
DFRobotDFPlayerMini dfPlayer;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

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

const int SW1 = 36;
const int SW2 = 39;
const int SW3 = 34;
const int SW4 = 35;

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

//Defining the behavior
// Flasher led1(ledpin#, on time, off time);
Flasher led1(LED_1, 250, 250);                          // LED 1 blinks 0.5 period and 50% duty cycle
Flasher led2(LED_2, 1200 * 20 / 100, 1200 * 80 / 100);  // LED 2 blinks 1.2 period and 20% duty cycle
Flasher led3(LED_3, 1400 * 62 / 100, 1400 * 38 / 100);  // LED 3 blinks 1.4 period and 62% duty cycle
Flasher led4(LED_4, 3000 * 55 / 100, 3000 * 45 / 100);  // LED 4 blinks 3.0 period and 55% duty cycle
Flasher led5(LED_5, 7500 * 10 / 100, 7500 * 90 / 100);  // LED 5 blinks 7.5 period and 10% duty cycle

// The Setup

void setup() {
  pinMode(SW1, INPUT);  // sets the digital pin 36 as input
  pinMode(SW2, INPUT);  // sets the digital pin 39 as input
  pinMode(SW3, INPUT);  // sets the digital pin 34 as input
  pinMode(SW4, INPUT);  // sets the digital pin 35 as input

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
  initOLED();
  splashOLED();
  initDFP();
  splashDFPlayer();
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

  // Set GPIO2 as an OUTPUT
  pinMode(23, OUTPUT);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

  server.serveStatic("/", LittleFS, "/");

  // Route to set GPIO state to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_1, HIGH);
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

  // Route to set GPIO state to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_1, LOW);
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

  // Route to set GPIO state to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_2, HIGH);
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

  // Route to set GPIO state to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_2, LOW);
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

    // Route to set GPIO state to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_3, HIGH);
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

  // Route to set GPIO state to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_3, LOW);
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

    // Route to set GPIO state to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_4, HIGH);
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

  // Route to set GPIO state to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_4, LOW);
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

    // Route to set GPIO state to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_5, HIGH);
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

  // Route to set GPIO state to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(LED_5, LOW);
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });


  // Start server
  server.begin();

  ElegantOTA.begin(&server);  // Start ElegantOTA

  // End of ELegant OTA Setup
}
//end setup()

void loop() {
  if (availableDFPLAYER) {
    if (!digitalRead(SW1)) {
      dfPlayer.play(1);
      Serial.print(digitalRead(SW1));
      Serial.println(", playing track 1");
    }
    if (!digitalRead(SW2)) {
      dfPlayer.play(2);
      Serial.print(digitalRead(SW2));
      Serial.println(", playing track 2");
    }
    if (!digitalRead(SW3)) {
      dfPlayer.play(3);
      Serial.print(digitalRead(SW3));
      Serial.println(", playing track 3");
    }
    if (!digitalRead(SW4)) {
      dfPlayer.play(5);
      Serial.print(digitalRead(SW4));
      Serial.println(", playing track 5");

    
    }
  }
  // displayAlarm(); // check OLED i2cTest code and fix// OLED is not displaying the alarm message when the SW of emergency is trigerred


  wink();  // Heart beat aka activity indicator LED function.
  led1.Update();
  led2.Update();
  led3.Update();
  led4.Update();
  led5.Update();
}  //end loop()
