#ifndef MQTTMAIN_H
#define MQTTMAIN_H

#include <MQTT.h>

extern MQTTClient client;

#define CLIENT_NAME_MQTT "PMD_DEVICE"  // Adjustable at runtime in future if needed

// These can eventually be loaded from /mqtt.json
extern String PUBLISHING_TOPIC;
extern String SUBSCRIPTION_TOPIC;

void connect();
void messageReceived(String& topic, String& payload);
void publishDevicePresence();

#endif
