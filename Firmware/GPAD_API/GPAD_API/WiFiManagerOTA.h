#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <WiFiManager.h>
#include <LittleFS.h>
#include <vector>

extern const char *DEFAULT_SSID;
extern String ledState;
extern int WiFiLed;

namespace WifiOTA
{
    class Manager
    {
    public:
        struct Credential
        {
            String ssid;
            String password;
        };

        Manager(WiFiClass &wifi, Print &print);
        ~Manager();

        void initialize();
        void connect(const char *const accessPointSsid);
        void setConnectedCallback(std::function<void()> callBack);
        void setApStartedCallback(std::function<void()> callback);
        wifi_mode_t getMode();
        IPAddress getAddress();
        bool saveCredentials(const String &ssid, const String &password);
        bool loadCredentials(String &ssid, String &password);
        bool loadCredentialsList(std::vector<Credential> &credentials);

    private:
        WiFiClass &wifi;
        Print &print;
        WiFiManager wifiManager;
        std::function<void()> connectedCallback;
        std::function<void()> apStartedCallback;

        void ssidSaved();
        void ipSet();
        void apStarted();
        bool connectStoredCredentials(const String &ssid, const String &password, unsigned long timeoutMs = 15000);
        void startPortal(const char *const accessPointSsid);
    };

    void initLittleFS();
    String processor(const String &var);
};

#endif // WIFI_MANAGER_H
