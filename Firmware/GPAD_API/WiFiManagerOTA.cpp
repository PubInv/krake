#include "WiFiManagerOTA.h"
#include <LittleFS.h>

const char* default_ssid = "ESP32-Setup";  // Default AP Name
String ssid = "";
String password = "";
String ledState = "";
int WiFiLed = 2;  // Modify based on actual LED pin

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
    #if (DEBUG >1)
    Serial.println("LittleFS mounted successfully.");
    #endif 
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
