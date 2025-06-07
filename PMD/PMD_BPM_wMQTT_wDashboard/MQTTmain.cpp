#include "MQTTmain.h"
#include <WiFi.h>
#include <MQTT.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>


String PUBLISHING_TOPIC = "/default/pub";
String SUBSCRIPTION_TOPIC = "/default/sub";

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");

  String mac = WiFi.macAddress();
  mac.replace(":", "");

  // Set Last Will
  String statusTopic = "/PMD/" + mac + "/status";
  String willPayload = "{\"online\": false}";
  client.setWill(statusTopic.c_str(), willPayload.c_str(), true, 1);

  int attempts = 0;
  while (!client.connect(CLIENT_NAME_MQTT, "public", "public")) {
    if (++attempts > 10) {
      Serial.println("🚫 MQTT connection failed too many times. Restarting...");
      ESP.restart();
    }
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\n✅ MQTT connected!");

  // Publish device URL to unique topic
  String urlTopic = "url/" + mac;
  String url = "http://" + WiFi.localIP().toString();
  client.publish(urlTopic.c_str(), url.c_str(), true);

  // Subscribe to your topic
  client.subscribe(SUBSCRIPTION_TOPIC);
}

void messageReceived(String& topic, String& payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  if (topic.startsWith("bpm/")) {
    String deviceID = topic.substring(4);
    int bpmValue = payload.toInt();
    Serial.printf("Got BPM from %s: %d\n", deviceID.c_str(), bpmValue);
  }
}

void publishDevicePresence() {
  if (WiFi.status() != WL_CONNECTED || !client.connected()) return;

  String mac = WiFi.macAddress();
  mac.replace(":", "");

  String topic = "/PMD/" + mac + "/status";

  StaticJsonDocument<256> doc;
  doc["ip"] = WiFi.localIP().toString();
  doc["mac"] = mac;
  doc["type"] = "PMD";
  doc["online"] = true;

  char payload[256];
  serializeJson(doc, payload);

  client.publish(topic.c_str(), payload, true);
  Serial.printf("📡 MQTT presence to topic: %s\n", topic.c_str());
  Serial.println("📡 MQTT presence sent: " + String(payload));
}
