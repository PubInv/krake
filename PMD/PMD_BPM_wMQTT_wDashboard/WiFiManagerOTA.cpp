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
  if (LittleFS.exists("/wifi.json")) {
    File file = LittleFS.open("/wifi.json", "r");
    if (file) {
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, file);
      if (!error) {
        ssid = doc["ssid"].as<String>();
        password = doc["password"].as<String>();
        file.close();

        WiFi.begin(ssid.c_str(), password.c_str());
        Serial.println("Connecting using saved /wifi.json ...");
        unsigned long startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
          Serial.print(".");
          delay(500);
        }

        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("\n✅ Connected using /wifi.json");
          return;
        }
        Serial.println("\n⚠️ Failed to connect with saved WiFi. Falling back to WiFiManager.");
      } else {
        Serial.println("❌ Failed to parse /wifi.json");
      }
    } else {
      Serial.println("❌ Failed to open /wifi.json");
    }
  }

  // Fallback to WiFiManager AP Mode
  WiFiManager wifiManager;
  if (!wifiManager.autoConnect(default_ssid)) {
    Serial.println("Failed to connect. Restarting...");
    delay(2000);
    ESP.restart();
  }

  ssid = WiFi.SSID();
  password = WiFi.psk();

  // Save to /wifi.json
  StaticJsonDocument<256> doc;
  doc["ssid"] = ssid;
  doc["password"] = password;
  File file = LittleFS.open("/wifi.json", "w");
  if (file) {
    serializeJson(doc, file);
    file.close();
    Serial.println("✅ WiFi credentials saved to /wifi.json");
  }
}


void initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("An error occurred while mounting LittleFS.");
  } else {
    Serial.println("LittleFS mounted successfully.");
  }
}

void initWiFi() {
  if (LittleFS.exists("/wifi.json")) {
    File file = LittleFS.open("/wifi.json", "r");
    if (file) {
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, file);
      if (!error) {
        String savedSSID = doc["ssid"].as<String>();
        String savedPass = doc["password"].as<String>();
        Serial.printf("📡 Connecting to saved SSID: %s\n", savedSSID.c_str());
        WiFi.begin(savedSSID.c_str(), savedPass.c_str());

        unsigned long startTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
          delay(500);
          Serial.print(".");
        }

        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("\n✅ WiFi connected from /wifi.json");
          return;
        } else {
          Serial.println("\n⚠️ Failed to connect from /wifi.json. You may need to reboot into config mode.");
        }
      } else {
        Serial.println("❌ Failed to parse /wifi.json");
      }
      file.close();
    } else {
      Serial.println("❌ Could not open /wifi.json");
    }
  } else {
    Serial.println("📁 No /wifi.json found");
  }

  // Do not start AP here — this is only a passive connection attempt.
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
