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
        Manager();
        ~Manager();

        void connect(const char *const accessPointSsid, bool nonBlocking);
        bool process();

    private:
        bool _nonBlocking = false;
        WiFiManager wifiManager;
    };

    void initLittleFS();
    String processor(const String &var);
};

#endif // WIFI_MANAGER_H
