#include "WiFiManagerOTA.h"
#include <LittleFS.h>

const char *DEFAULT_SSID = "ESP32-Setup"; // Default AP Name
String ssid_wf = "";
String password_wf = "";
String ledState = "";
int WiFiLed = 2; // Modify based on actual LED pin

using namespace WifiOTA;

void testFunction(std::function<void(int, int)> callback)
{
  callback(1, 2);
}

Manager::Manager(WiFiClass &wifi) : wifi(wifi) {}
Manager::~Manager() {}

void Manager::connect(const char *const accessPointSsid, bool nonBlocking)
{
  this->nonBlocking = nonBlocking;

  // According to WifiManager's documentation, best practice is still to set the WiFi mode manually
  // https://github.com/tzapu/WiFiManager/blob/master/examples/Basic/Basic.ino#L5
  this->wifi.mode(WIFI_STA);

  this->wifiManager.setConfigPortalBlocking(!nonBlocking);
  this->wifiManager.setConfigPortalTimeout(45);

  WiFiClass &thisWifi = this->wifi;
  auto saveConfigCallback = [&]()
  {
    Manager::ssidSaved(thisWifi);
  };

  this->wifiManager.setSaveConfigCallback(saveConfigCallback);

  auto thisConnectedCallback = this->connectedCallback;
  auto apStaConnectedCallback = [&](arduino_event_id_t event, arduino_event_info_t info)
  {
    Serial.print("Connected to Network: ");
    Serial.print(thisWifi.SSID());
    Serial.print("\n");

    Serial.print("Obtained IP Address: ");
    thisWifi.localIP().printTo(Serial);
    Serial.print("\n");

    thisConnectedCallback();
  };
  this->wifi.onEvent(apStaConnectedCallback, arduino_event_id_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

  bool connectSuccess = false;
  if (accessPointSsid == "")
  {
    connectSuccess = this->wifiManager.autoConnect(DEFAULT_SSID);
  }
  else
  {
    connectSuccess = this->wifiManager.autoConnect(accessPointSsid);
  }
}

void Manager::process()
{
  if (this->nonBlocking)
  {
    this->wifiManager.process();
  }
}

void Manager::ssidSaved(WiFiClass &wifi)
{
  Serial.print("Network Saved with SSID: ");
  Serial.print(wifi.SSID());
  Serial.print("\n");
}

void WifiOTA::initLittleFS()
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

void Manager::setConnectedCallback(std::function<void()> callback)
{
  this->connectedCallback = callback;
}

String WifiOTA::processor(const String &var)
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
