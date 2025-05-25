#ifndef MQTTMAIN_H
#define MQTTMAIN_H

#include <WiFi.h>  // for WiFiClient
#include <MQTT.h>  // for MQTTClient
#include <WiFiClient.h>

extern WiFiClient net;
extern MQTTClient client;

#define BROKER "public.cloud.shiftr.io"
#define CLIENT_NAME_MQTT "PubInv"

//#define Nagham
#define LEE
//#define Robert

#ifdef Nagham
// LB0005
#define SUBSCRIPTION_TOPIC "F024F9F1B880_ACK"
#define PUBLISHING_TOPIC "F024F9F1B880_ALM"
#endif
// #define Robert
#ifdef Robert
// US0003
#define SUBSCRIPTION_TOPIC "ECC9FF7C8C98_ACK"
#define PUBLISHING_TOPIC "ECC9FF7C8C98_ALM"
#endif

//#define LEE
#ifdef LEE  
// US0005
#define SUBSCRIPTION_TOPIC "ECC9FF7D8EE8_ACK"
#define PUBLISHING_TOPIC "ECC9FF7D8EE8_ALM"
#endif

#define PUBLISHING_RATE 10000


// ==== Function Prototypes ====
void connect();
void messageReceived(String& topic, String& payload);

#endif
