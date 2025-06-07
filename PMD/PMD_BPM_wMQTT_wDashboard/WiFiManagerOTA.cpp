#include "WiFiManagerOTA.h"
#include <LittleFS.h>
#include "ID.h"
#include <ArduinoJson.h>
#include "WebServerManager.h"

const char* default_ssid = DEVICE_UNDER_TEST;  // Default AP Name
String ssid = "";
String password = "";
String ledState = "";
// const int WiFiLed = 2;  // Modify based on actual LED pin

extern String mqttBroker;
String mqttPubTopic = "default/pub";
String mqttSubTopic = "default/sub";
String deviceRole = "PMD";  // Default

void saveCredentials(const char* ssid, const char* password) {
  File file = LittleFS.open("/wifi.txt", "w");
  if (file) {
    file.println(ssid);
    file.println(password);
    file.close();
    Serial.println("WiFi credentials saved.");
  } else {
    Serial.println("Failed to save WiFi credentials.");
  }
}

bool loadCredentials() {
  File file = LittleFS.open("/wifi.txt", "r");
  if (!file) {
    Serial.println("No saved WiFi credentials found.");
    return false;
  }

  ssid = file.readStringUntil('\n');
  password = file.readStringUntil('\n');
  ssid.trim();
  password.trim();

  file.close();
  Serial.println("Loaded WiFi credentials from storage.");
  return true;
}

void WiFiMan() {
  WiFiManager wifiManager;

  if (!loadCredentials()) {
    if (!wifiManager.autoConnect(default_ssid)) {
      Serial.println("Failed to connect. Restarting...");
      ESP.restart();
    }
    ssid = WiFi.SSID();
    password = WiFi.psk();
    saveCredentials(ssid.c_str(), password.c_str());
  }

  Serial.println("Connected to WiFi!");
}

void initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("An error occurred while mounting LittleFS.");
  } else {
    Serial.println("LittleFS mounted successfully.");
  }
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("\nConnected. IP Address: " + WiFi.localIP().toString());
}

// void saveMQTTConfig(const String& broker, const String& pub, const String& sub, const String& role) {
//   StaticJsonDocument<256> doc;
//   doc["broker"] = broker;
//   doc["pub"] = pub;
//   doc["sub"] = sub;
//   doc["role"] = role;

//   File file = LittleFS.open("/mqtt.json", "w");
//   if (file) {
//     serializeJson(doc, file);
//     file.close();
//     Serial.println("✅ MQTT config saved.");
//   } else {
//     Serial.println("❌ Failed to save MQTT config.");
//   }
// }

bool loadMQTTConfig() {
  if (!LittleFS.exists("/mqtt.json")) {
    Serial.println("⚠️ No /mqtt.json file found.");
    return false;
  }

  File file = LittleFS.open("/mqtt.json", "r");
  if (!file) {
    Serial.println("❌ Failed to open /mqtt.json for reading.");
    return false;
  }

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("❌ Failed to parse /mqtt.json.");
    return false;
  }

  mqttBroker = doc["broker"] | mqttBroker;
  mqttPubTopic = doc["pub"] | mqttPubTopic;
  mqttSubTopic = doc["sub"] | mqttSubTopic;
  deviceRole = doc["role"] | deviceRole;

  Serial.println("✅ MQTT config loaded:");
  Serial.println(" Broker: " + mqttBroker);
  Serial.println(" Pub: " + mqttPubTopic);
  Serial.println(" Sub: " + mqttSubTopic);
  Serial.println(" Role: " + deviceRole);

  return true;
}


String processor(const String& var) {
  if (var == "STATE") {
    if (digitalRead(WiFiLed)) {
      ledState = "ON";
    } else {
      ledState = "OFF";
    }
    return ledState;
  }
  return String();
}
