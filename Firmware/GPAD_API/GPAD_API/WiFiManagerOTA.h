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

namespace WifiOTA
{
    class Manager
    {
    public:
        Manager(const char *const accessPointSsid, bool nonBlocking);
        ~Manager();

        bool process();

    private:
        bool nonBlocking;
        WiFiManager wifiManager;
    };

    String processor(const String &var);
};

void WiFiMan(const char *accessPointSsid);
void initLittleFS();

#endif // WIFI_MANAGER_H
