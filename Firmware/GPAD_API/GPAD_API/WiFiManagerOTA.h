#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <WiFiManager.h>
#include <LittleFS.h>

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
        static const size_t MAX_SAVED_WIFI_NETWORKS = 20;
        struct CredentialList
        {
            Credential items[MAX_SAVED_WIFI_NETWORKS];
            size_t count;
        };
        typedef void (*Callback)();

        Manager(WiFiClass &wifi, Print &print);
        ~Manager();

        void initialize();
        void connect(const char *const accessPointSsid);
        void process();
        void setConnectedCallback(Callback callBack);
        void setApStartedCallback(Callback callback);
        wifi_mode_t getMode();
        IPAddress getAddress();
        void startConfigPortal(const char *const accessPointSsid, unsigned long timeoutSeconds);
        bool forgetSavedCredentials();
        bool saveCredentials(const String &ssid, const String &password);
        bool loadCredentials(String &ssid, String &password);
        bool loadCredentialsList(CredentialList &credentials);
        bool hasSavedCredentials();

    private:
        WiFiClass &wifi;
        Print &print;
        WiFiManager wifiManager;
        Callback connectedCallback;
        Callback apStartedCallback;
        CredentialList storedCredentials;
        String recoveryPortalSsid;
        size_t nextStoredCredentialIndex;
        unsigned long storedCredentialsStartedMs;
        unsigned long storedCredentialAttemptStartedMs;
        bool storedCredentialAttemptActive;
        bool recoveryPortalStarted;
        bool connectionNotified;

        void ssidSaved();
        void ipSet();
        void apStarted();
        void beginNextStoredCredential();
        void startPortal(const char *const accessPointSsid);
    };

    bool initLittleFS();
    bool isLittleFSMounted();
    void printLittleFSDiagnostics(Print &print);
    String processor(const String &var);
};

#endif // WIFI_MANAGER_H
