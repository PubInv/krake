#ifndef MQTTMAIN_H
#define MQTTMAIN_H

#include <WiFi.h>           // for WiFiClient
#include <MQTT.h>           // for MQTTClient
#include <WiFiClient.h>

extern WiFiClient net;
extern MQTTClient client;

#define BROKER "public.cloud.shiftr.io"
#define CLIENT_NAME_MQTT "PubInv"
#define SUBSCRIPTION_TOPIC "F024F9F1B880_ACK"
#define PUBLISHING_TOPIC "F024F9F1B880_ALM"
#define PUBLISHING_RATE 30000


// ==== Function Prototypes ====
void connect();
void messageReceived(String& topic, String& payload);

#endif
