#include "WiFiManagerOTA.h"
#include <LittleFS.h>

namespace
{
  const char *WIFI_CREDENTIALS_PATH = "/wifi.json";

  String jsonEscape(const String &value)
  {
    String escaped;
    escaped.reserve(value.length() + 8);
    for (size_t i = 0; i < value.length(); ++i)
    {
      const char ch = value.charAt(i);
      if (ch == '\\' || ch == '"')
      {
        escaped += '\\';
      }
      escaped += ch;
    }
    return escaped;
  }

  bool extractJsonString(const String &json, const char *key, String &value)
  {
    const String keyToken = String("\"") + key + "\"";
    const int keyPos = json.indexOf(keyToken);
    if (keyPos < 0)
    {
      return false;
    }

    const int colonPos = json.indexOf(':', keyPos + keyToken.length());
    if (colonPos < 0)
    {
      return false;
    }

    int quoteStart = json.indexOf('"', colonPos + 1);
    if (quoteStart < 0)
    {
      return false;
    }

    String parsed;
    bool escaped = false;
    for (int i = quoteStart + 1; i < json.length(); ++i)
    {
      const char ch = json.charAt(i);
      if (escaped)
      {
        parsed += ch;
        escaped = false;
        continue;
      }
      if (ch == '\\')
      {
        escaped = true;
        continue;
      }
      if (ch == '"')
      {
        value = parsed;
        return true;
      }
      parsed += ch;
    }
    return false;
  }
}

const char *DEFAULT_SSID = "ESP32-Setup"; // Default AP Name
String ssid_wf = "";
String password_wf = "";
String ledState = "";
int WiFiLed = 2; // Modify based on actual LED pin

using namespace WifiOTA;

Manager::Manager(WiFiClass &wifi, Print &print)
    : wifi(wifi), print(print)
{
}

void Manager::initialize()
{
  // According to WifiManager's documentation, best practice is still to set the WiFi mode manually
  // https://github.com/tzapu/WiFiManager/blob/master/examples/Basic/Basic.ino#L5
  this->wifi.mode(WIFI_STA);
}

Manager::~Manager() {}

void Manager::connect(const char *const accessPointSsid)
{

  String savedSsid;
  String savedPassword;
  if (this->loadCredentials(savedSsid, savedPassword) && this->connectStoredCredentials(savedSsid, savedPassword))
  {
    this->print.println("Connected using stored wifi.json credentials.");
    this->ipSet();
    return;
  }

  this->startPortal(accessPointSsid);
}

void Manager::startPortal(const char *const accessPointSsid)
{

  auto saveConfigCallback = [this]()
  {
    this->ssidSaved();
  };

  this->wifiManager.setSaveConfigCallback(saveConfigCallback);

  auto apStartedCallback = [this](WiFiManager *wifiManager)
  {
    this->apStarted();
  };

  this->wifiManager.setAPCallback(apStartedCallback);

  auto staGotIpCallback = [this](arduino_event_id_t event, arduino_event_info_t info)
  {
    if ((this->wifi.localIP() == INADDR_NONE) && (this->getMode() == wifi_mode_t::WIFI_MODE_STA))
    {
      return;
    }

    this->ipSet();
  };
  this->wifi.onEvent(staGotIpCallback, arduino_event_id_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

  bool connectSuccess = false;
  if (accessPointSsid == nullptr || accessPointSsid[0] == '\0')
  {
    connectSuccess = this->wifiManager.autoConnect(DEFAULT_SSID);
  }
  else
  {
    connectSuccess = this->wifiManager.autoConnect(accessPointSsid);
  }

  if (!connectSuccess)
  {
    this->print.println("WiFiManager portal completed without connection.");
  }
}

void Manager::setConnectedCallback(std::function<void()> callback)
{
  this->connectedCallback = callback;
}

void Manager::setApStartedCallback(std::function<void()> callback)
{
  this->apStartedCallback = callback;
}

wifi_mode_t Manager::getMode()
{
  return this->wifi.getMode();
}

IPAddress Manager::getAddress()
{
  switch (this->getMode())
  {
  case wifi_mode_t::WIFI_MODE_AP:
    return this->wifi.softAPIP();
  case wifi_mode_t::WIFI_MODE_STA:
    return this->wifi.localIP();
  default:
    return INADDR_NONE;
  }
}

bool Manager::saveCredentials(const String &ssid, const String &password)
{
  File file = LittleFS.open(WIFI_CREDENTIALS_PATH, "w");
  if (!file)
  {
    this->print.println("Failed to open wifi.json for writing.");
    return false;
  }

  String payload = "{\"ssid\":\"";
  payload += jsonEscape(ssid);
  payload += "\",\"password\":\"";
  payload += jsonEscape(password);
  payload += "\"}";

  const size_t written = file.print(payload);
  file.close();
  return written == payload.length();
}

bool Manager::loadCredentials(String &ssid, String &password)
{
  ssid = "";
  password = "";
  if (!LittleFS.exists(WIFI_CREDENTIALS_PATH))
  {
    return false;
  }

  File file = LittleFS.open(WIFI_CREDENTIALS_PATH, "r");
  if (!file)
  {
    this->print.println("Failed to open wifi.json for reading.");
    return false;
  }

  String content = file.readString();
  file.close();

  String loadedSsid;
  String loadedPassword;
  if (!extractJsonString(content, "ssid", loadedSsid) || !extractJsonString(content, "password", loadedPassword))
  {
    this->print.println("wifi.json missing required keys.");
    return false;
  }

  loadedSsid.trim();
  if (loadedSsid.length() == 0)
  {
    this->print.println("wifi.json has empty SSID.");
    return false;
  }

  ssid = loadedSsid;
  password = loadedPassword;
  return true;
}

bool Manager::connectStoredCredentials(const String &ssid, const String &password, unsigned long timeoutMs)
{
  this->print.print("Attempting connection from wifi.json SSID: ");
  this->print.println(ssid);

  this->wifi.begin(ssid.c_str(), password.c_str());
  const unsigned long startMs = millis();
  while ((millis() - startMs) < timeoutMs)
  {
    if (this->wifi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(250);
  }

  this->print.println("Stored wifi.json credentials failed to connect.");
  this->wifi.disconnect(true, false);
  return false;
}

void Manager::ssidSaved()
{
  this->print.print("Network Saved with SSID: ");
  this->print.print(this->wifi.SSID());
  this->print.print("\n");
}

void Manager::ipSet()
{
  this->print.print("Connected to Network: ");
  this->print.print(this->wifi.SSID());
  this->print.print("\n");

  this->print.print("Obtained IP Address: ");
  this->wifi.localIP().printTo(Serial);
  this->print.print("\n");

  if (this->connectedCallback)
  {
    this->connectedCallback();
  }
}

void Manager::apStarted()
{
  this->print.print("AP Has Started: ");
  this->wifi.softAPIP().printTo(this->print);
  this->print.print("\n");

  if (this->apStartedCallback)
  {
    this->apStartedCallback();
  }
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
