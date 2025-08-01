// Author: Nagham Kheir
// Date: 20250316
// A PMD BootUp, Similar to Do Nothing but Serial Splash, LCD Test, LCD Splash,   BootButton Test for stuck key. Loop() has only non blocking code.
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
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DailyStruggleButton.h>
#include "WiFiManagerOTA.h"
#include "Button.h"
#include "OLEDDisplay.h"
#include "Wink.h"
#include "BPM.h"
#include "MQTTmain.h"
#include <MQTT.h>
#include "ID.h"
#include <Adafruit_SSD1306.h>


#define BAUDRATE 115200  //Serial port \
                         // Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

bool clearOTA = false;


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
CenteredBarsDisplay WiFIbars(&display);


// ==== MQTT Networking ====
WiFiClient net;
MQTTClient client;
// ==== Timers ====
unsigned long lastMillis = 0;

void notifyClients(const String &message) {
  ws.textAll(message);
}

// Some PMD Hardware

// Pins for switches and LEDs and more / HMWK2
#define BOOT_BUTTON 0
const int LED_BUILTIN = 2;  // Built-in LED on ESP32
const int LED_1 = 15;
const int LED_2 = 4;
const int LED_3 = 5;
const int LED_4 = 18;
const int LED_5 = 19;
int WiFiLed = 23;

extern int syntheticBPM;
#define potINPUT 32

// Pins for switches and LEDs and more //Krake
// #define BOOT_BUTTON 0
// const int LED_BUILTIN = 13;  //Krake
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
PulseCounter pulse(33, 15, 2041, 60000);  // (sensorPin, ledPin, threshold, interval)
int myBPM = pulse.update();

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

int16_t rowPosition = 2;
int16_t columnPosition = 0;
const int16_t rowHeight = 8;  // Just a guess



void initOLED() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    return;
  }
  display.clearDisplay();
  display.display();
}


void splashOLED_P1() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  int row = 10;
  display.setCursor(0, row);
  display.println("Public");
  row += 20;
  display.setCursor(0, row);
  display.println("Invention");
  display.display();
  delay(2000);
}


void splashOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  int row = 0, rowHeight = 10;
  display.setCursor(0, row);
  display.println(PROG_NAME);
  row += rowHeight;
  display.setCursor(0, row);
  display.println(VERSION);
  row += rowHeight;
  display.setCursor(0, row);
  display.println(F("Compiled at:"));
  row += rowHeight;
  display.setCursor(0, row);
  display.println(F(__DATE__ " " __TIME__));
  row += rowHeight;
  display.setCursor(0, row);
  display.println("IP: " + WiFi.localIP().toString());
  display.display();
  delay(3000);
}

void updateOLED() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.print("BPM= ");
  display.println(myBPM);
  WiFIbars.drawCenteredHorizontalBars(110, 23);
  display.display();
}

void setup() {

  const int LED_PINS[] = { LED_1, LED_2, LED_3, LED_4, LED_5 };

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

  pinMode(potINPUT, INPUT);  //A potentiometer, 3.3 to GND for user input.

  Wire.begin();
  initOLED();  //OLED ready!
  splashOLED_P1();
  // // WiFi including OTA.
  // //Report Start of WiFi setup
  // display.println("WiFi setup");
  // rowPosition += rowHeight;
  // display.setCursor(0, rowPosition);
  // display.display();

  WiFiMan();
  initWiFi();
  initLittleFS();
  setupOTA();
  server.begin();             // Start web page server
  ElegantOTA.begin(&server);  // Start ElegantOTA

  //   //Report Start of MQTT client
  //   splashOLED();
  // display.println("Connected to MQTT");
  // rowPosition += rowHeight;
  // display.setCursor(0, rowPosition);
  // display.display();

  client.setWill(PUBLISHING_TOPIC, "a5 PMD device is disconnected from the broker. STALE DATA.", false, 2);

  client.setKeepAlive(60);

  //MQTT client connection
  client.begin(BROKER, net);

  client.onMessage(messageReceived);
  connect();

  //   //Report Start of ???
  //   splashOLED();
  //   display.println("?????");
  //    rowPosition += rowHeight;
  //   display.setCursor(0, rowPosition);
  //   display.display();


  setupButton();  //Buttons, switches
  pulse.begin();
  splashOLED();

  // More setup code here
  digitalWrite(LED_1, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_2, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_3, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_4, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_5, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_BUILTIN, LOW);  //Make built in LED low at end of setup.

}  //end setup()

void loop() {
  int16_t rowHeight = 8;  // Just a guess
  static int myBPM = 0;
  wink();        // Heart beat aka activity indicator LED function.
  loopButton();  // poles all the buttons.

  // myBPM = pulse.update();

  client.loop();
  delay(10);

  if (!client.connected()) {
    connect();
  }

  // Check BPM, or synthetic BPM and publish
  if (millis() - lastMillis > PUBLISHING_RATE) {
    lastMillis = millis();
    //    client.publish(PUBLISHING_TOPIC, "myBPM= " + (char)myBPM);
    // client.publish(PUBLISHING_TOPIC, "a1mysyntheticBPM= " + String(analogRead(potINPUT)));

    syntheticBPM = map(analogRead(potINPUT), 0, 4095, 35, 210);
    //client.publish(PUBLISHING_TOPIC, "a1mysyntheticBPM= " + String(syntheticBPM));

    /////// missing code, count BPM

    //Syntheic heart rate from bottons on HOMEWORK 2 assembly
    //Low, Normal, warning and panic heart rates
    const int THRESHOLD_Bradycardi = 60;
    const int THRESHOLD_Tachycardia = 100;

    if (0 == syntheticBPM) {
      Serial.println("No BPM detected.");
    } else if (syntheticBPM < THRESHOLD_Bradycardi) {
      //Low bradycardia.
      Serial.print("Bradycardi, ");
      Serial.print(syntheticBPM);
      Serial.println("BPM");
      client.publish(PUBLISHING_TOPIC, "a4 Bradycardi, Low mysyntheticBPM= " + String(syntheticBPM));

    } else if (syntheticBPM < THRESHOLD_Tachycardia) {
      //Normal between 60 and 100
      Serial.print("Normal range, ");
      Serial.print(syntheticBPM);
      Serial.println("BPM");
      client.publish(PUBLISHING_TOPIC, "a1 Normal BPM= " + String(syntheticBPM));
    } else {
      //Warning heart rate  tachycardia
      Serial.print("Tachycardia, ");
      Serial.print(syntheticBPM);
      Serial.println("BPM");
      client.publish(PUBLISHING_TOPIC, "a5 Tachycardia, High mysyntheticBPM= " + String(syntheticBPM));
    }

    // //MQTT message
    // if (messageToPublish) {
    //   client.publish(PUBLISHING_TOPIC, messageToPublish);

    //   messageToPublish = nullptr;  // Reset after publishing
    // }
    // foo client.publish(PUBLISHING_TOPIC, "a1mysyntheticBPM= " + String(analogRead(potINPUT)));

  }  // end MQTT publishing

  updateOLED();

  //Check of clearOTA.
  if (true == clearOTA) {
    clearOTA = false;  // Only call once per press of the button.
    ElegantOTA.clearAuth();
    Serial.println("ElegantOTA.clearAuth()");
  }

}  //end loop()
