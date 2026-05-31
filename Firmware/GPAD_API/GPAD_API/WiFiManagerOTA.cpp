#include "WiFiManagerOTA.h"
#include <LittleFS.h>
#include <Preferences.h>
#include "gpad_utility.h"

namespace
{
  const char *WIFI_CREDENTIALS_PATH = "/wifi.json";
  const char *WIFI_PREFERENCES_NAMESPACE = "krake-wifi";
  const char *WIFI_PREFERENCES_KEY = "networks";
  const unsigned long WIFI_PORTAL_TIMEOUT_SECONDS = 60;
  const unsigned long WIFI_STORED_CREDENTIALS_BUDGET_MS = 15000;
  const unsigned long WIFI_STORED_CREDENTIAL_ATTEMPT_MS = 5000;
  bool littleFsMounted = false;

  bool saveCredentialsToPreferences(const String &payload)
  {
    Preferences preferences;
    if (!preferences.begin(WIFI_PREFERENCES_NAMESPACE, false))
    {
      return false;
    }
    const size_t written = preferences.putString(WIFI_PREFERENCES_KEY, payload);
    preferences.end();
    return written > 0;
  }

  String loadCredentialsFromPreferences()
  {
    Preferences preferences;
    if (!preferences.begin(WIFI_PREFERENCES_NAMESPACE, true))
    {
      return String();
    }
    const String payload = preferences.getString(WIFI_PREFERENCES_KEY, "");
    preferences.end();
    return payload;
  }

  bool clearCredentialsFromPreferences()
  {
    Preferences preferences;
    if (!preferences.begin(WIFI_PREFERENCES_NAMESPACE, false))
    {
      return false;
    }
    const bool cleared = !preferences.isKey(WIFI_PREFERENCES_KEY) || preferences.remove(WIFI_PREFERENCES_KEY);
    preferences.end();
    return cleared;
  }

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
    : wifi(wifi), print(print), connectedCallback(nullptr), apStartedCallback(nullptr),
      nextStoredCredentialIndex(0), storedCredentialsStartedMs(0), storedCredentialAttemptStartedMs(0),
      storedCredentialAttemptActive(false), recoveryPortalStarted(false), connectionNotified(false)
{
  this->storedCredentials.count = 0;
}

void Manager::initialize()
{
  // According to WifiManager's documentation, best practice is still to set the WiFi mode manually
  // https://github.com/tzapu/WiFiManager/blob/master/examples/Basic/Basic.ino#L5
  this->wifi.mode(WIFI_STA);
  // WiFi recovery must never stop the alarm/UI loop while a network is unavailable.
  this->wifiManager.setConfigPortalTimeout(WIFI_PORTAL_TIMEOUT_SECONDS);
  this->wifiManager.setConfigPortalBlocking(false);

  auto staGotIpCallback = [this](arduino_event_id_t event, arduino_event_info_t info)
  {
    (void)event;
    (void)info;
    this->ipSet();
  };
  this->wifi.onEvent(staGotIpCallback, arduino_event_id_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
}

Manager::~Manager() {}

void Manager::connect(const char *const accessPointSsid)
{
  this->recoveryPortalSsid = (accessPointSsid == nullptr || accessPointSsid[0] == '\0')
                                 ? DEFAULT_SSID
                                 : accessPointSsid;
  this->storedCredentials.count = 0;
  this->nextStoredCredentialIndex = 0;
  this->storedCredentialsStartedMs = millis();
  this->storedCredentialAttemptStartedMs = 0;
  this->storedCredentialAttemptActive = false;
  this->recoveryPortalStarted = false;
  this->connectionNotified = false;

  if (this->loadCredentialsList(this->storedCredentials) && this->storedCredentials.count > 0)
  {
    this->beginNextStoredCredential();
    return;
  }

  this->print.println(F("Starting non-blocking WiFi recovery portal."));
  this->startPortal(this->recoveryPortalSsid.c_str());
}

void Manager::process()
{
  const wl_status_t status = this->wifi.status();
  if (status == WL_CONNECTED)
  {
    this->storedCredentialAttemptActive = false;
    this->ipSet();
  }
  else
  {
    this->connectionNotified = false;
  }

  if (this->recoveryPortalStarted)
  {
    this->wifiManager.process();
    return;
  }

  if (!this->storedCredentialAttemptActive || status == WL_CONNECTED)
  {
    return;
  }

  const unsigned long now = millis();
  if (!millisIntervalElapsed(now, this->storedCredentialAttemptStartedMs, WIFI_STORED_CREDENTIAL_ATTEMPT_MS) &&
      elapsedMillis(now, this->storedCredentialsStartedMs) < WIFI_STORED_CREDENTIALS_BUDGET_MS)
  {
    return;
  }

  this->print.println(F("Stored WiFi credentials failed to connect."));
  this->wifi.disconnect(true, false);
  this->storedCredentialAttemptActive = false;
  this->beginNextStoredCredential();
}

void Manager::beginNextStoredCredential()
{
  if (this->nextStoredCredentialIndex >= this->storedCredentials.count ||
      elapsedMillis(millis(), this->storedCredentialsStartedMs) >= WIFI_STORED_CREDENTIALS_BUDGET_MS)
  {
    this->print.println(F("Stored WiFi retry budget exhausted; starting recovery portal."));
    this->startPortal(this->recoveryPortalSsid.c_str());
    return;
  }

  const Credential &credential = this->storedCredentials.items[this->nextStoredCredentialIndex++];
#if (DEBUG_LEVEL > 0)
  this->print.print(F("Trying saved WiFi "));
  this->print.print(this->nextStoredCredentialIndex);
  this->print.print(F("/"));
  this->print.print(this->storedCredentials.count);
  this->print.print(F(": "));
  this->print.println(credential.ssid);
#endif
  this->wifi.begin(credential.ssid.c_str(), credential.password.c_str());
  this->storedCredentialAttemptStartedMs = millis();
  this->storedCredentialAttemptActive = true;
}

void Manager::startPortal(const char *const accessPointSsid)
{
  if (this->recoveryPortalStarted)
  {
    return;
  }

  auto saveConfigCallback = [this]()
  {
    this->ssidSaved();
  };
  this->wifiManager.setSaveConfigCallback(saveConfigCallback);

  auto apCallback = [this](WiFiManager *wifiManager)
  {
    (void)wifiManager;
    this->apStarted();
  };
  this->wifiManager.setAPCallback(apCallback);

  this->recoveryPortalStarted = true;
  this->wifiManager.startConfigPortal((accessPointSsid == nullptr || accessPointSsid[0] == '\0') ? DEFAULT_SSID : accessPointSsid);
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
  const bool preferencesCleared = clearCredentialsFromPreferences();
  bool mirrorCleared = true;
  if (littleFsMounted && LittleFS.exists(WIFI_CREDENTIALS_PATH))
  {
    mirrorCleared = LittleFS.remove(WIFI_CREDENTIALS_PATH);
  }
  return preferencesCleared && mirrorCleared;
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

  if (!saveCredentialsToPreferences(payload))
  {
    this->print.println(F("Failed to save WiFi credentials to NVS."));
    return false;
  }

  // Keep a LittleFS mirror for backward compatibility and diagnostics. The NVS
  // copy remains authoritative because uploading a filesystem image erases this file.
  if (littleFsMounted)
  {
    File file = LittleFS.open(WIFI_CREDENTIALS_PATH, "w");
    if (!file)
    {
      this->print.println(F("Warning: failed to mirror WiFi credentials to wifi.json."));
    }
    else
    {
      file.print(payload);
      file.close();
    }
  }
  return true;
}

bool Manager::hasSavedCredentials()
{
  return loadCredentialsFromPreferences().length() > 0 ||
         (littleFsMounted && LittleFS.exists(WIFI_CREDENTIALS_PATH));
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
  String content = loadCredentialsFromPreferences();
  if (content.length() == 0 && littleFsMounted && LittleFS.exists(WIFI_CREDENTIALS_PATH))
  {
    File file = LittleFS.open(WIFI_CREDENTIALS_PATH, "r");
    if (!file)
    {
      this->print.println(F("Failed to open wifi.json for reading."));
      return false;
    }
    content = file.readString();
    file.close();
    if (content.length() > 0)
    {
      saveCredentialsToPreferences(content);
    }
  }
  if (content.length() == 0)
  {
    return false;
  }

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
    this->print.println(F("Stored WiFi credentials are missing required keys."));
    return false;
  }

  legacySsid.trim();
  legacyPassword.trim();
  if (legacySsid.length() == 0 || legacyPassword.length() == 0)
  {
    this->print.println(F("Stored WiFi credentials have an invalid SSID/password."));
    return false;
  }

  credentials.items[credentials.count++] = {legacySsid, legacyPassword};
  return true;
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
  if (this->connectionNotified || this->wifi.status() != WL_CONNECTED)
  {
    return;
  }
  this->connectionNotified = true;

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

bool WifiOTA::initLittleFS()
{
  littleFsMounted = LittleFS.begin(false);
  if (!littleFsMounted)
  {
    Serial.println(F("LittleFS mount failed. Filesystem unavailable; refusing to auto-format."));
    Serial.println(F("Upload the LittleFS image or format explicitly during service."));
    return false;
  }

  printLittleFSDiagnostics(Serial);
  return true;
}

bool WifiOTA::isLittleFSMounted()
{
  return littleFsMounted;
}

void WifiOTA::printLittleFSDiagnostics(Print &print)
{
  print.println(F("=== LittleFS diagnostics ==="));
  print.print(F("Mounted: "));
  print.println(littleFsMounted ? F("yes") : F("no"));
  if (!littleFsMounted)
  {
    print.println(F("============================"));
    return;
  }

  print.print(F("Total bytes: "));
  print.println(LittleFS.totalBytes());
  print.print(F("Used bytes: "));
  print.println(LittleFS.usedBytes());

  File root = LittleFS.open("/");
  if (!root || !root.isDirectory())
  {
    print.println(F("Unable to list filesystem root."));
    print.println(F("============================"));
    return;
  }

  File file = root.openNextFile();
  if (!file)
  {
    print.println(F("Filesystem root is empty. Upload the LittleFS image."));
  }
  while (file)
  {
    print.print(F("  "));
    print.print(file.name());
    print.print(F(" ("));
    print.print(file.size());
    print.println(F(" bytes)"));
    file = root.openNextFile();
  }
  print.println(F("============================"));
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
