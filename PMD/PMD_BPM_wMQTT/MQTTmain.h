#ifndef MQTTMAIN_H
#define MQTTMAIN_H

#include <WiFi.h>  // for WiFiClient
#include <MQTT.h>  // for MQTTClient
#include <WiFiClient.h>

extern WiFiClient net;
extern MQTTClient client;

#define BROKER "public.cloud.shiftr.io"
#define CLIENT_NAME_MQTT "PubInv"

// //#define Nagham
// //#define LEE
// //#define Robert
// #define Event


// #ifdef Event

#define SUBSCRIPTION_TOPIC "F024F9F1B880_ACK"  // LB0005 : mac
#define PUBLISHING_TOPIC "F024F9F1B880_ALM"    // LB0005
#define SUBSCRIPTION_TOPIC "ECC9FF7D8EF4_ACK"  // US0004
#define PUBLISHING_TOPIC "ECC9FF7D8EF4_ALM"    // US0004
#define SUBSCRIPTION_TOPIC "ECC9FF7D8F00_ACK"  // US0002
#define PUBLISHING_TOPIC "ECC9FF7D8F00_ALM"    // US0002
#define SUBSCRIPTION_TOPIC "ECC9FF7D8EE8_ACK"  // US0005
#define PUBLISHING_TOPIC "ECC9FF7D8EE8_ALM"    // US0005

// #endif


// #ifdef Nagham
// // LB0005
// #define SUBSCRIPTION_TOPIC "F024F9F1B880_ACK"
// #define PUBLISHING_TOPIC "F024F9F1B880_ALM"
// #endif


// #ifdef Robert
// // US0003
// #define SUBSCRIPTION_TOPIC "ECC9FF7C8C98_ACK"
// #define PUBLISHING_TOPIC "ECC9FF7C8C98_ALM"
// #endif


// #ifdef LEE
// // US0005
// #define SUBSCRIPTION_TOPIC "ECC9FF7D8EE8_ACK"
// #define PUBLISHING_TOPIC "ECC9FF7D8EE8_ALM"
// #endif

#define PUBLISHING_RATE 10000


// // ==== Function Prototypes ====
void connect();
void messageReceived(String& topic, String& payload);

#endif
