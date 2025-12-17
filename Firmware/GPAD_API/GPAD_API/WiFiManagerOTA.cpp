#include "WiFiManagerOTA.h"
#include <LittleFS.h>

const char *DEFAULT_SSID = "ESP32-Setup"; // Default AP Name
String ssid_wf = "";
String password_wf = "";
String ledState = "";
int WiFiLed = 2; // Modify based on actual LED pin

void WiFiMan(const char *accessPointSsid)
{
  /// According to WifiManager's documentation, best practice is still to set the WiFi mode manually
  /// https://github.com/tzapu/WiFiManager/blob/master/examples/Basic/Basic.ino#L5
  WiFi.mode(WIFI_STA);
  WiFiManager wifiManager;

  boolean connectSuccess = false;

  if (accessPointSsid == "")
  {
    connectSuccess = wifiManager.autoConnect(DEFAULT_SSID);
  }
  else
  {
    connectSuccess = wifiManager.autoConnect(accessPointSsid);
  }

  if (!connectSuccess)
  {
    Serial.println("Failed to connect. Restarting...");
    ESP.restart();
  }

  Serial.println("Connected to WiFi!");
}

void initLittleFS()
{
  if (!LittleFS.begin(true))
  {
    Serial.println("An error occurred while mounting LittleFS.");
  }
  else
  {
#if (DEBUG > 1)
    Serial.println("LittleFS mounted successfully.");
#endif
  }
}

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_wf.c_str(), password_wf.c_str());
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("\nConnected. IP Address: " + WiFi.localIP().toString());
}

String processor(const String &var)
{
  if (var == "STATE")
  {
    if (digitalRead(WiFiLed))
    {
      ledState = "ON";
    }
    else
    {
      ledState = "OFF";
    }
    return ledState;
  }
  return String();
}
