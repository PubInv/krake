// Author: Nagham Kheir
// Date: 20250316
// Fixed and cleaned version of PMD_BPM_wMQTT with functional BPM display
#include <WiFiManager.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <ElegantOTA.h>
#include <FS.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DailyStruggleButton.h>
#include <LittleFS.h>
#include "WiFiManagerOTA.h"
#include "Button.h"
#include "OLEDDisplay.h"
#include "Wink.h"
#include "BPM.h"
#include "MQTTmain.h"
#include <MQTT.h>
#include "ID.h"
#include "WebServerManager.h"
// #include "BPMLogger.h"
#define BAUDRATE 115200

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define SENSOR_PIN 33  // Analog input for pulse sensor
#define LED_PIN 15
#define BPM_THRESHOLD 1700
#define potINPUT 32
#define LED_BUILTIN 2
#define PIN_POT 32  // Analog input for potentiometer
// AsyncWebServer server(80);
// AsyncWebSocket ws("/ws");
WiFiClient net;
MQTTClient client;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
CenteredBarsDisplay WiFIbars(&display);
// PulseCounter pulse(SENSOR_PIN, LED_PIN, BPM_THRESHOLD, &display);
PulseCounter pulse(SENSOR_PIN, LED_PIN, &display);


bool clearOTA = false;
unsigned long lastMillis = 0;
int myBPM = 0;
extern int syntheticBPM;
// int threshold();
const int LED_1 = 15, LED_2 = 4, LED_3 = 5, LED_4 = 18, LED_5 = 19;
const int LED_PINS[] = { LED_1, LED_2, LED_3, LED_4, LED_5 };
const int LED_COUNT = sizeof(LED_PINS) / sizeof(LED_PINS[0]);
int WiFiLed = 23;
// int PotentionmeterPin = 32;
unsigned long lastBPMLogTime = 0;
const unsigned long BPMlogInterval = 10000;  // 10 seconds
unsigned long lastBpmSend = 0;
const unsigned long bpmInterval = 5000;
void notifyClients(const String &message) {
  ws.textAll(message);
}

void initOLED() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true) delay(1000);
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

// void updateOLED() {
//   display.clearDisplay();
//   display.setTextSize(2);
//   display.setTextColor(WHITE);
//   display.setCursor(0, 10);
//   display.print("BPM= ");
//   display.println(myBPM);
//   WiFIbars.drawCenteredHorizontalBars(110, 23);


//   display.display();
// }

void displayBPMandThreshold(int bpm, int threshold) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 18);
  display.print("BPM: ");
  display.println(bpm);
  WiFIbars.drawCenteredHorizontalBars(110, 23);
  display.setTextSize(1.2);
  display.setCursor(0, 42);
  display.print("Threshold: ");
  display.println(threshold);


  display.setCursor(0, 52);
  display.print("Analog: ");
  display.println(analogRead(33));

  display.display();
}


void setupOTA() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });
  server.serveStatic("/", LittleFS, "/");
}


// void BPMLogger_loop() {
//   unsigned long now = millis();

//   if (now - lastBPMLogTime >= BPMlogInterval) {
//     lastBPMLogTime = now;

//     logBPM(myBPM);

//   }
// }


void publishDeviceURL() {
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  String url = "http://" + WiFi.localIP().toString();
  client.publish(("url/" + mac).c_str(), url.c_str(), true);
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
  // initBPMLogger();
  Wire.begin();
  initOLED();


  splashOLED_P1();
  splashserial();
  pinMode(potINPUT, INPUT);

  for (int i = 0; i < LED_COUNT; ++i) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
  pinMode(WiFiLed, OUTPUT);
  digitalWrite(WiFiLed, HIGH);

  WiFiMan();
  initWiFi();
  initLittleFS();
  setupOTA();
  server.begin();
  ElegantOTA.begin(&server);
  client.setWill(PUBLISHING_TOPIC, "a5 PMD disconnected", false, 2);
  client.setKeepAlive(60);
  client.begin(BROKER, net);
  client.onMessage(messageReceived);
  connect();
  publishDeviceURL();
  setupButton();
  pulse.begin();
  splashOLED();
  setupWebServer();
  // updateOLED();

  pulse.setPotentiometerPin(PIN_POT);
  pulse.begin();


  digitalWrite(LED_1, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_2, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_3, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_4, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_5, LOW);        //Make built in LED low at end of setup.
  digitalWrite(LED_BUILTIN, LOW);  //Make built in LED low at end of setup.
}  //end setup()

void loop() {
  wink();
  loopButton();
  myBPM = pulse.update();
  client.loop();
  delay(10);

  if (!client.connected()) connect();

  if (millis() - lastMillis > PUBLISHING_RATE) {
    lastMillis = millis();
    syntheticBPM = map(analogRead(potINPUT), 0, 4095, 35, 210);
    if (syntheticBPM < 60) {
      client.publish(PUBLISHING_TOPIC, "a4 Bradycardia: " + String(syntheticBPM));
    } else if (syntheticBPM < 100) {
      client.publish(PUBLISHING_TOPIC, "a1 Normal BPM: " + String(syntheticBPM));
    } else {
      client.publish(PUBLISHING_TOPIC, "a5 Tachycardia: " + String(syntheticBPM));
    }
  }

  pulse.updateThresholdFromPot();  // Update threshold from potentiometer
  int bpm = pulse.update();        // Update BPM if new beat detected

  // if (myBPM > 50 && myBPM <= 120) {
  //   updateOLED();
  // }
  displayBPMandThreshold(pulse.getLastBPM(), pulse.getThreshold());

  // BPMLogger_loop();
  // if (clearOTA) {
  //   clearOTA = false;
  //   ElegantOTA.clearAuth();
  //   Serial.println("ElegantOTA.clearAuth()");
  // }

  if (millis() - lastBpmSend > bpmInterval) {
    lastBpmSend = millis();

    String topic = "bpm/" + WiFi.macAddress();  // or use an alias/device ID
    String payload = String(myBPM);

    if (client.connected()) {                                // handle this from the Krake side to display the bpm level wherever is needed
      client.publish(topic.c_str(), payload.c_str(), true);  // retain BPM
    }
  }

  // readAndPrintAnalog(32);
  // delay(1000);



}  // end loop
