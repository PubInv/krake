#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;
extern AsyncWebSocket ws;

void setupWebServer();
void setupStaticRoutes();
void setupAPIHandlers();
void loadMQTTConfig();

extern String mqttBroker;
extern String mqttTopic;
extern String messageS1, messageS2, messageS3, messageS4;

#endif