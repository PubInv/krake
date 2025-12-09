#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

// #include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <FS.h>
#include <LittleFS.h>

extern const char *DEFAULT_SSID;
// extern String ssid;
// extern String password;
extern String ledState;
extern int WiFiLed;

void saveCredentials(const char *ssid, const char *password);
bool loadCredentials();
void WiFiMan(const char *accessPointSsid);
void initLittleFS();
void initWiFi();
String processor(const String &var);

#endif // WIFI_MANAGER_H
