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

  bool parseJsonStringAt(const String &json, int keyPos, String &value, int *valueEndPos = nullptr)
  {
    if (keyPos < 0)
    {
      return false;
    }

    const int colonPos = json.indexOf(':', keyPos);
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
        if (valueEndPos != nullptr)
        {
          *valueEndPos = i;
        }
        return true;
      }
      parsed += ch;
    }
    return false;
  }

  bool extractJsonString(const String &json, const char *key, String &value, int startPos = 0, int *valueEndPos = nullptr)
  {
    const String keyToken = String("\"") + key + "\"";
    const int keyPos = json.indexOf(keyToken, startPos);
    if (keyPos < 0)
    {
      return false;
    }
    return parseJsonStringAt(json, keyPos + keyToken.length(), value, valueEndPos);
  }
}

const char *DEFAULT_SSID = "ESP32-Setup"; // Default AP Name
String ssid_wf = "";
String password_wf = "";
String ledState = "";
int WiFiLed = 2; // Modify based on actual LED pin

using namespace WifiOTA;

Manager::Manager(WiFiClass &wifi, Print &print)
    : wifi(wifi), print(print), connectedCallback(nullptr), apStartedCallback(nullptr)
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
  CredentialList credentials;
  if (this->loadCredentialsList(credentials))
  {
    for (size_t index = 0; index < credentials.count; ++index)
    {
#if (DEBUG_LEVEL > 0)
      this->print.print(F("Trying saved WiFi "));
      this->print.print(index + 1);
      this->print.print(F("/"));
      this->print.print(credentials.count);
      this->print.print(F(": "));
      this->print.println(credentials.items[index].ssid);
#endif

      if (this->connectStoredCredentials(credentials.items[index].ssid, credentials.items[index].password))
      {
        this->print.println(F("Connected using stored wifi.json credentials."));
        this->ipSet();
        return;
      }
    }
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
    this->print.println(F("WiFiManager portal completed without connection."));
  }
}

void Manager::setConnectedCallback(Callback callback)
{
  this->connectedCallback = callback;
}

void Manager::setApStartedCallback(Callback callback)
{
  this->apStartedCallback = callback;
}

wifi_mode_t Manager::getMode()
{
  return this->wifi.getMode();
}

void Manager::startConfigPortal(const char *const accessPointSsid, unsigned long timeoutSeconds)
{
  this->wifiManager.setConfigPortalTimeout(timeoutSeconds);
  this->startPortal(accessPointSsid);
}

bool Manager::forgetSavedCredentials()
{
  this->wifiManager.resetSettings();
  if (LittleFS.exists(WIFI_CREDENTIALS_PATH))
  {
    return LittleFS.remove(WIFI_CREDENTIALS_PATH);
  }
  return true;
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
  String trimmedSsid = ssid;
  String trimmedPassword = password;
  trimmedSsid.trim();
  trimmedPassword.trim();
  if (trimmedSsid.length() == 0 || trimmedPassword.length() == 0)
  {
    this->print.println(F("SSID and password are required."));
    return false;
  }

  CredentialList credentials;
  this->loadCredentialsList(credentials);

  CredentialList updated;
  updated.count = 0;

  updated.items[updated.count++] = {trimmedSsid, trimmedPassword};
  for (size_t i = 0; i < credentials.count; ++i)
  {
    if (credentials.items[i].ssid != trimmedSsid && updated.count < MAX_SAVED_WIFI_NETWORKS)
    {
      updated.items[updated.count++] = credentials.items[i];
    }
  }

  File file = LittleFS.open(WIFI_CREDENTIALS_PATH, "w");
  if (!file)
  {
    this->print.println(F("Failed to open wifi.json for writing."));
    return false;
  }

  String payload = "{\"networks\":[";
  for (size_t i = 0; i < updated.count; ++i)
  {
    if (i > 0)
    {
      payload += ",";
    }
    payload += "{\"ssid\":\"";
    payload += jsonEscape(updated.items[i].ssid);
    payload += "\",\"password\":\"";
    payload += jsonEscape(updated.items[i].password);
    payload += "\"}";
  }
  payload += "]}";

  const size_t written = file.print(payload);
  file.close();
  return written == payload.length();
}

bool Manager::loadCredentials(String &ssid, String &password)
{
  CredentialList credentials;
  if (!this->loadCredentialsList(credentials) || credentials.count == 0)
  {
    return false;
  }

  ssid = credentials.items[0].ssid;
  password = credentials.items[0].password;
  return true;
}

bool Manager::loadCredentialsList(CredentialList &credentials)
{
  credentials.count = 0;
  if (!LittleFS.exists(WIFI_CREDENTIALS_PATH))
  {
    return false;
  }

  File file = LittleFS.open(WIFI_CREDENTIALS_PATH, "r");
  if (!file)
  {
    this->print.println(F("Failed to open wifi.json for reading."));
    return false;
  }

  const String content = file.readString();
  file.close();

  int searchPos = 0;
  while (credentials.count < MAX_SAVED_WIFI_NETWORKS)
  {
    const int ssidKeyPos = content.indexOf("\"ssid\"", searchPos);
    if (ssidKeyPos < 0)
    {
      break;
    }

    String loadedSsid;
    int ssidEndPos = -1;
    if (!extractJsonString(content, "ssid", loadedSsid, ssidKeyPos, &ssidEndPos))
    {
      break;
    }

    const int passwordKeyPos = content.indexOf("\"password\"", ssidEndPos);
    if (passwordKeyPos < 0)
    {
      break;
    }

    String loadedPassword;
    int passwordEndPos = -1;
    if (!extractJsonString(content, "password", loadedPassword, passwordKeyPos, &passwordEndPos))
    {
      break;
    }

    loadedSsid.trim();
    loadedPassword.trim();
    if (loadedSsid.length() > 0 && loadedPassword.length() > 0)
    {
      bool alreadyExists = false;
      for (size_t i = 0; i < credentials.count; ++i)
      {
        if (credentials.items[i].ssid == loadedSsid)
        {
          alreadyExists = true;
          break;
        }
      }

      if (!alreadyExists)
      {
        credentials.items[credentials.count++] = {loadedSsid, loadedPassword};
      }
    }

    searchPos = passwordEndPos + 1;
  }

  if (credentials.count > 0)
  {
    return true;
  }

  // Backward compatibility with legacy single-network format.
  String legacySsid;
  String legacyPassword;
  if (!extractJsonString(content, "ssid", legacySsid) || !extractJsonString(content, "password", legacyPassword))
  {
    this->print.println(F("wifi.json missing required keys."));
    return false;
  }

  legacySsid.trim();
  legacyPassword.trim();
  if (legacySsid.length() == 0 || legacyPassword.length() == 0)
  {
    this->print.println(F("wifi.json has invalid SSID/password."));
    return false;
  }

  credentials.items[credentials.count++] = {legacySsid, legacyPassword};
  return true;
}

bool Manager::connectStoredCredentials(const String &ssid, const String &password, unsigned long timeoutMs)
{
#if (DEBUG_LEVEL > 0)
  this->print.print(F("Attempting connection from wifi.json SSID: "));
  this->print.println(ssid);
#endif

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

  this->print.println(F("Stored wifi.json credentials failed to connect."));
  this->wifi.disconnect(true, false);
  return false;
}

void Manager::ssidSaved()
{
#if (DEBUG_LEVEL > 0)
  this->print.print(F("Network Saved with SSID: "));
  this->print.print(this->wifi.SSID());
  this->print.print(F("\n"));
#endif
}

void Manager::ipSet()
{
#if (DEBUG_LEVEL > 0)
  this->print.print(F("Connected to Network: "));
  this->print.print(this->wifi.SSID());
  this->print.print(F("\n"));

  this->print.print(F("Obtained IP Address: "));
  this->wifi.localIP().printTo(Serial);
  this->print.print(F("\n"));
#endif

  if (this->connectedCallback)
  {
    this->connectedCallback();
  }
}

void Manager::apStarted()
{
#if (DEBUG_LEVEL > 0)
  this->print.print(F("AP Has Started: "));
  this->wifi.softAPIP().printTo(this->print);
  this->print.print(F("\n"));
#endif

  if (this->apStartedCallback)
  {
    this->apStartedCallback();
  }
}

void WifiOTA::initLittleFS()
{
  if (!LittleFS.begin(true))
  {
    Serial.println(F("An error occurred while mounting LittleFS."));
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
