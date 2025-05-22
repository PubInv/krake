#define PROG_NAME "MQTT_timing"
#define VERSION " V.0.0.1 "
#define MODEL_NAME "Model: HW2_WiFiReady_Elegant"
#define DEVICE_UNDER_TEST "SN: 00001"
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "LB"


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

int debugLevel = 0; //Incrase to make more verbose.

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
// const char* ssid = "readfamilynetwork";
// const char* password = "magicalsparrow96";


WiFiClient net;
MQTTClient client;

String StringreceivedMQTT = "";  // For the call back event
bool receivedMQTT = false;

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
#define ClientName "mqtt_timing"
//  while (!client.connect("arduino", "public", "public")) {
//  while (!client.connect("mqtt_timing", "public", "public")) {
  while (!client.connect(ClientName, "public", "public")) {
    Serial.print(".");
    delay(1000);
  }

if (debugLevel >2)  {
   Serial.println("\n WiFi connected to " + String(ssid) + ".");
}
 
  // client.subscribe("/hello");
  client.subscribe(PROG_NAME);

  // client.unsubscribe("/hello");
}


void messageReceived(String& topic, String& payload) {
  //Serial.println("incoming: " + topic + " - " + payload);
  Serial.print(payload);
  Serial.print(", ");
  Serial.print(millis());
  Serial.print(", ");

if (debugLevel >2)  {
  Serial.print(", Difference= "); 
}
  unsigned long theLong = strtol(payload.c_str(), NULL, 10);
  unsigned long theDifference = (millis() - theLong);
  Serial.println(theDifference);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}// end MessageRecevied

void splashserial() {
  Serial.println(F("==================================="));
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.println(MODEL_NAME);
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(F("Compiled at: "));
  Serial.println(F(__DATE__ " " __TIME__));
  Serial.println(LICENSE);
  Serial.println(F("==================================="));
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  splashserial();

  WiFi.begin(ssid, password);
  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("public.cloud.shiftr.io", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    // client.publish(PROG_NAME, "sent time " + String(millis()));
    // client.publish(PROG_NAME, "sent time " );
    client.publish(PROG_NAME, String(millis()));
  }
}
