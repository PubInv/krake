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

  Serial.print("\nconnecting...");
  while (!client.connect(CLIENT_NAME_MQTT, "public", "public")) {


    // client.setWill(PUBLISHING_TOPIC, "a5 PMD device is disconnected from the broker. STALE DATA.", false, 2);

    // client.setKeepAlive(60);


    Serial.print(".");
    delay(1000);
  }





  Serial.println("\nconnected!");
  client.subscribe(SUBSCRIPTION_TOPIC);
}

void messageReceived(String& topic, String& payload) {
  Serial.println("incoming: " + topic + " - " + payload);
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
