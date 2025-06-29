 
const char* default_ssid = "ESP32-Setup";  // Default AP Name
String ssid = "";
String password = "";

// Stores LED state
String ledState;

// Function to save WiFi credentials to LittleFS
void saveCredentials(const char* ssid, const char* password) {
  File file = LittleFS.open("/wifi.txt", "w");
  if (file) {
    file.println(ssid);
    file.println(password);
    file.close();
    Serial.println("WiFi credentials saved.");
  } else {
    Serial.println("Failed to save WiFi credentials.");
  }
}

// Function to load WiFi credentials from LittleFS
bool loadCredentials() {
  File file = LittleFS.open("/wifi.txt", "r");
  if (!file) {
    Serial.println("No saved WiFi credentials found.");
    return false;
  }
  
  ssid = file.readStringUntil('\n');
  password = file.readStringUntil('\n');
  ssid.trim();
  password.trim();
  
  file.close();
  Serial.println("Loaded WiFi credentials from storage.");
  return true;
}

// WiFiManager Setup
void WiFiMan() {
  WiFiManager wifiManager;

  // If no saved credentials, enter configuration mode
  if (!loadCredentials()) {
    if (!wifiManager.autoConnect(default_ssid)) {
      Serial.println("Failed to connect. Restarting...");
      ESP.restart();
    }
    ssid = WiFi.SSID();
    password = WiFi.psk();
    saveCredentials(ssid.c_str(), password.c_str());
  }
  
  Serial.println("Connected to WiFi!");
}

// Initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("An error occurred while mounting LittleFS.");
  } else {
    Serial.println("LittleFS mounted successfully.");
  }
}

// Initialize WiFi using stored credentials
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("\nConnected. IP Address: " + WiFi.localIP().toString());
}

// Replaces placeholder with LED state value
String processor(const String& var) {
  if (var == "STATE") {
    if (digitalRead(WiFiLed)) {
      ledState = "ON";
    } else {
      ledState = "OFF";
    }
    return ledState;
  }
  return String();
}


