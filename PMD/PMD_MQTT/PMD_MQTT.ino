// Customized this by changing these defines
#define PROG_NAME "PMD_MQTT"
#define VERSION "V0.0.1 "
#define MODEL_NAME "Model: PMD_HOMEWORK2"
#define DEVICE_UNDER_TEST "SN: xxxxx"  //A Serial Number
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "LB"
#define BAUDRATE 115200  //Serial port

// Program Name: MQTT_cpp_h
// Date: 2025 05 09
//
// This example uses an ESP32 Development Board
// to connect to shiftr.io.
//
// You can check on your device after a successful
// connection here: https://www.shiftr.io/try.
//
// by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <WiFi.h>
#include <MQTT.h>

//Aley network
const char* ssid = "Home";
const char* password = "adt@1963#";

//Maryville network
// const char* ssid = "VRX";
// const char* password = "textinsert";

//Houstin network
// const char* ssid = "DOS_WIFI";
// const char* password = "$Suve07$$";

// // Austin network
// const char* ssid = "readfamilynetwork";
// const char* password = "magicalsparrow96";

WiFiClient net;
MQTTClient client;


unsigned long lastMillis = 0;


void splashserial() {
  //Serial splash
  Serial.println(F("==================================="));
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.println(MODEL_NAME);
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(F("Compiled at: "));
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a  
  Serial.println(LICENSE);
  Serial.println(F("==================================="));
  Serial.println();
}




void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "public", "public")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/hello");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {

  const int LED_BUILTIN = 13;       // ESP32 Kit
  pinMode(LED_BUILTIN, OUTPUT);     // set the LED pin mode
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)

  Serial.begin(BAUDRATE);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB
  }

  WiFi.begin(ssid, password);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("public.cloud.shiftr.io", net);
  client.onMessage(messageReceived);

  connect();
  digitalWrite(LED_BUILTIN, LOW);  //Make built in LED low at end of setup.
}  //end setup()


void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("/hello", "world");
  }
}
