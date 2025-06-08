
#include "WebServerManager.h"
#include <LittleFS.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "ID.h"
#include "MQTTmain.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String mqttBroker = "public.cloud.shiftr.io";
String mqttTopic = "default/topic";
String messageS1 = "a2Check in please.";
String messageS2 = "Message from SW2";
String messageS3 = "a4Urgent Support Now.";
String messageS4 = "a5Send Emergency";
extern int myBPM;

void saveMQTTConfig(const String &broker, const String &pub, const String &sub, const String &role) {
  StaticJsonDocument<512> doc;
  doc["broker"] = broker;
  doc["pub"] = pub;
  doc["sub"] = sub;
  doc["role"] = role;

  File configFile = LittleFS.open("/mqtt.json", "w");
  if (!configFile) {
    Serial.println("❌ Failed to open /mqtt.json for writing");
    return;
  }

  if (serializeJson(doc, configFile) == 0) {
    Serial.println("❌ Failed to write JSON to /mqtt.json");
  } else {
    Serial.println("✅ MQTT config saved to /mqtt.json");
  }

  configFile.close();
}

void setupWebServer() {
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount Failed");
    return;
  }

  setupStaticRoutes();
  setupAPIHandlers();

  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client,
                AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      Serial.println("WebSocket client connected");
    }
  });
  server.addHandler(&ws);

  server.begin();
  Serial.println("Web server started.");
}

void setupStaticRoutes() {
  server.serveStatic("/", LittleFS, "/").setDefaultFile("monitor.html");

  server.serveStatic("/setup.html", LittleFS, "/setup.html");
  server.serveStatic("/monitor.html", LittleFS, "/monitor.html");
  server.serveStatic("/control.html", LittleFS, "/control.html");
  server.serveStatic("/update.html", LittleFS, "/update.html");
  server.serveStatic("/dashboard.html", LittleFS, "/dashboard.html");
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->redirect("/monitor.html");
  });

  server.serveStatic("/style.css", LittleFS, "/style.css");
}

void setupAPIHandlers() {
  server.on("/deviceinfo", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<256> doc;
    doc["version"] = VERSION;
    doc["name"] = "HW2_WiFiReady_Elegant";
    doc["mac"] = WiFi.macAddress();
    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
  });

  server.on("/devices", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<1024> doc;
    String deviceRole = "PMD"; // Default fallback
    JsonArray arr = doc.to<JsonArray>();
    JsonObject dev = arr.createNestedObject();
    dev["mac"] = WiFi.macAddress();
    dev["ip"] = WiFi.localIP().toString();
    dev["type"] = deviceRole;
    dev["online"] = true;
    dev["rssi"] = WiFi.RSSI();
    dev["lastSeen"] = millis() / 1000;  // seconds since boot
    deviceRole = doc["role"].as<String>();
    String result;
    serializeJson(doc, result);
    request->send(200, "application/json", result);
  });


  server.on("/bpm", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(myBPM));
  });

  server.on(
    "/config", HTTP_POST, [](AsyncWebServerRequest *request) {},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t, size_t) {
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, data);

      if (error) {
        request->send(400, "text/plain", "Invalid JSON");
        return;
      }

      mqttBroker = doc["broker"].as<String>();
      mqttTopic = doc["topic"].as<String>();
      String pub = doc["pub"] | "default/pub";
      String sub = doc["sub"] | "default/sub";
      String role = doc["role"] | "PMD";

      saveMQTTConfig(mqttBroker, pub, sub, role);
      request->send(200, "text/plain", "MQTT settings saved");
    });

  server.on("/mqtt-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    String status = client.connected() ? "🟢 Connected" : "🔴 Disconnected";
    request->send(200, "text/plain", status);
  });

  server.on(
    "/messages", HTTP_POST, [](AsyncWebServerRequest *request) {},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t, size_t) {
      StaticJsonDocument<512> doc;
      deserializeJson(doc, data);
      messageS1 = doc["S1"].as<String>();
      messageS2 = doc["S2"].as<String>();
      messageS3 = doc["S3"].as<String>();
      messageS4 = doc["S4"].as<String>();
      request->send(200, "text/plain", "Messages saved");
    });

  server.on("/rssi", HTTP_GET, [](AsyncWebServerRequest *request) {
    int rssi = WiFi.RSSI();
    request->send(200, "text/plain", String(rssi));
  });

  server.on("/url", HTTP_GET, [](AsyncWebServerRequest *request) {
    String url = "http://" + WiFi.localIP().toString();
    request->send(200, "text/plain", url);
  });



  server.on(
    "/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t, size_t) {
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, data);

      if (error) {
        request->send(400, "text/plain", "Invalid JSON");
        return;
      }

      String newSSID = doc["ssid"];
      String newPass = doc["password"];

      StaticJsonDocument<256> wifiDoc;
      wifiDoc["ssid"] = newSSID;
      wifiDoc["password"] = newPass;

      File wifiFile = LittleFS.open("/wifi.json", "w");
      if (!wifiFile) {
        request->send(500, "text/plain", "Failed to open file");
        return;
      }

      serializeJson(wifiDoc, wifiFile);
      wifiFile.close();

      request->send(200, "text/plain", "WiFi credentials saved. Please reboot to apply.");
    });

  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
    int n = WiFi.scanNetworks();
    StaticJsonDocument<1024> doc;
    JsonArray arr = doc.createNestedArray("networks");

    for (int i = 0; i < n; i++) {
      JsonObject net = arr.createNestedObject();
      net["ssid"] = WiFi.SSID(i);
      net["rssi"] = WiFi.RSSI(i);
      net["secure"] = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
    }

    String result;
    serializeJson(doc, result);
    request->send(200, "application/json", result);
  });
}
