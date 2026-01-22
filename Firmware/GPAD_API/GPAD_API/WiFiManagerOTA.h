#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

// #include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <FS.h>
#include <LittleFS.h>

extern const char *DEFAULT_SSID;
extern String ledState;
extern int WiFiLed;

namespace WifiOTA
{
    class Manager
    {
    public:
        Manager(WiFiClass &wifi, Print &print);
        ~Manager();

        void connect(const char *const accessPointSsid);
        void setConnectedCallback(std::function<void()> callBack);

    private:
        WiFiClass &wifi;
        Print &print;
        WiFiManager wifiManager;
        std::function<void()> connectedCallback;

        void ssidSaved();
        void ipSet();
    };

    void initLittleFS();
    String processor(const String &var);
};

#endif // WIFI_MANAGER_H
