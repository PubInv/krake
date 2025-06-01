#include "WebServerManager.h"
#include <LittleFS.h>
#include <WiFi.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String mqttBroker = "public.cloud.shiftr.io";
String mqttTopic = "default/topic";
String messageS1 = "a2Check in please.";
String messageS2 = "Message from SW2";
String messageS3 = "a4Urgent Support Now.";
String messageS4 = "a5Send Emergency";

void setupWebServer() {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed");
    return;
  }

  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  server.serveStatic("/info.html", LittleFS, "/info.html");
  server.serveStatic("/mqtt.html", LittleFS, "/mqtt.html");
  server.serveStatic("/messages.html", LittleFS, "/messages.html");
  server.serveStatic("/style.css", LittleFS, "/style.css");

  server.on("/deviceinfo", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<256> doc;
    doc["version"] = "V.0.1.8";
    doc["name"] = "HW2_WiFiReady_Elegant";
    doc["mac"] = WiFi.macAddress();
    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
  });

  server.on("/mqttconfig", HTTP_POST, [](AsyncWebServerRequest *request) {},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t, size_t) {
      StaticJsonDocument<256> doc;
      deserializeJson(doc, data);
      mqttBroker = doc["broker"].as<String>();
      mqttTopic = doc["topic"].as<String>();
      request->send(200, "text/plain", "MQTT settings saved");
    }
  );

  server.on("/messages", HTTP_POST, [](AsyncWebServerRequest *request) {},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t, size_t) {
      StaticJsonDocument<512> doc;
      deserializeJson(doc, data);
      messageS1 = doc["S1"].as<String>();
      messageS2 = doc["S2"].as<String>();
      messageS3 = doc["S3"].as<String>();
      messageS4 = doc["S4"].as<String>();
      request->send(200, "text/plain", "Messages saved");
    }
  );

  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client,
                AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      Serial.println("WebSocket client connected");
    }
  });
  server.addHandler(&ws);

  server.begin();
}
