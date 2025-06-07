#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;
extern AsyncWebSocket ws;

extern String mqttBroker;
extern String mqttPubTopic;
extern String mqttSubTopic;
extern String deviceRole;

void saveMQTTConfig(const String& broker, const String& pub, const String& sub, const String& role);
bool loadMQTTConfig();

void setupWebServer();
void setupStaticRoutes();
void setupAPIHandlers();
// 

 extern String mqttTopic;
extern String messageS1, messageS2, messageS3, messageS4;

#endif