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
#include <WiFiManager.h>
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
#include "IFTTTNotifier.h"


#define BAUDRATE 115200  //Serial port \
                         // Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
CenteredBarsDisplay WiFIbars(&display);
// ==== MQTT Networking ====
WiFiClient net;
MQTTClient client;
WiFiManager wifiManager;
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
const int SW1_IFTT = 36;
extern int syntheticBPM;

const char *default_ssid = "ESP32-Setup";  // Default AP Name

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
PulseCounter pulse(39, 15, 2041, 60000);  // (sensorPin, ledPin, threshold, interval)
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


void initOLED() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    return;
  }
  display.clearDisplay();
  display.display();
}


void splashOLED() {


  int16_t row = 9, rowHeight = 10;
  display.clearDisplay();
  display.setTextSize(2.5);
  display.setTextColor(WHITE);
  display.setCursor(0, row);
  display.print("Public  ");
  row += rowHeight;
  display.print("  Invention");
  display.display();
  delay(5000);

 
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, row);
  display.println(PROG_NAME);
  row += rowHeight;
  display.println(VERSION);
  row += rowHeight;
  display.println(F("Compiled at:"));
  row += rowHeight;
  display.println(F(__DATE__ " " __TIME__));
  row += rowHeight;

  display.println("IP: " + WiFi.localIP().toString());
  row += rowHeight;
  display.print("myBPM= ");
  display.print(myBPM);
  display.display();



}  //end splashOLED

void updateOLED() {
  int16_t row = 0, rowHeight = 8;
  row += rowHeight;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, row);
  display.println(PROG_NAME);
  row += rowHeight;
  // display.println(VERSION);
  row += rowHeight;

  // display.println(F("Compiled at:"));
  row += rowHeight;
  // display.println(F(__DATE__ " " __TIME__));
  // row += rowHeight;

  display.println("IP: " + WiFi.localIP().toString());
  row += rowHeight;
  display.print("Pot Value= ");
  display.println(analogRead(potINPUT));
  row += rowHeight;

  display.print("my BPM= ");
  display.println(myBPM);
  row += rowHeight;

  display.print("syn_BPM= ");
  display.print(syntheticBPM);
  display.println("     ");  // Clear leftovers

  WiFIbars.drawCenteredHorizontalBars(110, 25);  // bottom right corner
  display.display();

  // // add conditional format for the wifi icon
  // if (!wifiManager.autoConnect(default_ssid)) {
  //   display.drawBitmap(0, 0, wifi_icon, 16, 16, WHITE);
  // } else {
  //   display.drawBitmap(0, 0, wifi_disconnected_icon, 16, 16, WHITE);
  // }


  display.display();
}  //end updateOLED

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
  pinMode(SW1_IFTT, INPUT);         // External pull-up is used


  Serial.begin(BAUDRATE);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB
  }

  splashserial();
  pinMode(potINPUT, INPUT);  //A potentiometer, 3.3 to GND for user input.

  Wire.begin();
  initOLED();
  splashOLED();
  WiFiMan();
  initWiFi();
  initLittleFS();
  setupOTA();
  server.begin();             // Start web page server
  ElegantOTA.begin(&server);  // Start ElegantOTA
  client.begin(BROKER, net);
  client.onMessage(messageReceived);
  connect();
  setupButton();  //Buttons, switches
  pulse.begin();

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

  if (millis() - lastMillis > PUBLISHING_RATE) {
    lastMillis = millis();
    client.publish(PUBLISHING_TOPIC, "myBPM= " + (char)myBPM);

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

    } else if (syntheticBPM < THRESHOLD_Tachycardia) {
      //Normal between 60 and 100
      Serial.print("Normal range, ");
      Serial.print(syntheticBPM);
      Serial.println("BPM");

    } else {
      //Warning heart rate  tachycardia
      Serial.print("Tachycardia, ");
      Serial.print(syntheticBPM);
      Serial.println("BPM");
    }

    //MQTT message
    if (messageToPublish) {
      client.publish(PUBLISHING_TOPIC, messageToPublish);

      messageToPublish = nullptr;  // Reset after publishing
    }
  }

  updateOLED();
  IFTTTloop();

}  //end loop()
