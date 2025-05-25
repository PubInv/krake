#include "MQTTmain.h"
#include <WiFi.h>
#include <MQTT.h>
#include <WiFiClient.h>

 
void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(CLIENT_NAME_MQTT, "public", "public")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
  client.subscribe(SUBSCRIPTION_TOPIC);
}

void messageReceived(String& topic, String& payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}
