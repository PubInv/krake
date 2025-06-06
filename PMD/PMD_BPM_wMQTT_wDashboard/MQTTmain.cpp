#include "MQTTmain.h"
#include <WiFi.h>
#include <MQTT.h>
#include <WiFiClient.h>
#include "MQTTClient.h"


void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nconnecting to WiFi...");

  while (!client.connect(CLIENT_NAME_MQTT, "public", "public")) {

    // client.setWill(PUBLISHING_TOPIC, "a5 PMD device is disconnected from the broker. STALE DATA.", false, 2);

    // client.setKeepAlive(60);
    String mac = WiFi.macAddress();  // e.g., "F0:24:F9:F1:B8:80"
    mac.replace(":", "");            // Remove colons for safe topic

    String topic = "url/" + mac;
    String url = "http://" + WiFi.localIP().toString();
    client.publish(topic.c_str(), url.c_str(), true);  // Retain message

    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
  client.subscribe(SUBSCRIPTION_TOPIC);
}

void messageReceived(String& topic, String& payload) {
  Serial.println("incoming: " + topic + " - " + payload);

    if (topic.startsWith("bpm/")) {
    String deviceID = topic.substring(4);  // extract mac or ID
    int bpmValue = payload.toInt();

    Serial.printf("Got BPM from %s: %d\n", deviceID.c_str(), bpmValue);

    // You can store it in a map or array if tracking multiple devices
    // deviceBPMs[deviceID] = bpmValue;
  }
}



 

// const char payload[]= "LastWillMessage: a5 PMD device is disconnected from the broker. STALE DATA."
// bool retained= false ;
// int qos= 2 ;
// int keepAlive = 60 ;
// // void setWill(const char topic[], const char payload[], bool retained, int qos);
// void setWill(const char topic[], const char payload[], bool retained, int qos);

// void setKeepAlive(int keepAlive);




//  setWill(PUBLISHING_TOPIC, "LastWillMessage: a5 PMD device is disconnected from the broker. STALE DATA.", false, 2);

// CleanSession= true ;

//  setKeepAlive(60);
