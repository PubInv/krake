#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <HTTPClient.h>
#include <DFRobotDFPlayerMini.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFiManager.h>

// Configuration and Pin Definitions
const char* server_address = "192.168.1.3";
const int serverPort = 5500;
const int analogPin = 34;
const int MUTE_BUTTON_PIN = 36;
const int ON_OFF_BUTTON_PIN = 39;
const int LED_PIN = 2;
const int lampPins[] = {15, 4, 5, 18, 19, 23}; // Emergency lamp pins including MUTE LED

const char* ssidAP = "ESP32-Access-Point";
const char* passwordAP = "123456789";
const byte DNS_PORT = 53;

// Global Variables
bool ledState = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
bool lastButtonState = HIGH;
bool currentButtonState;
bool buttonPressed = false;

HardwareSerial mySerial1(2); // Use UART2
DFRobotDFPlayerMini myDFPlayer;
LiquidCrystal_I2C lcd(0x3F, 20, 4);
WebServer server(80);
DNSServer dnsServer;

String ssid = "";
String password = "";
bool trackPlaying = false;
unsigned long previousMillis = 0;
const long interval = 100;

const char* htmlPage = R"rawliteral(
<html>
<body>
<h1>Enter Wi-Fi Credentials</h1>
<form action="/connect" method="POST">
SSID:<br><input type="text" name="ssid"><br>
Password:<br><input type="password" name="password"><br><br>
<input type="submit" value="Submit">
</form>
</body>
</html>)rawliteral";

void setup() {
    Serial.begin(115200);
    mySerial1.begin(9600, SERIAL_8N1, 16, 17);

    pinMode(MUTE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(ON_OFF_BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    for (int pin : lampPins) {
        pinMode(pin, OUTPUT);
    }

    WiFi.softAP(ssidAP, passwordAP);
    Serial.println("Access Point Created");

    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    server.on("/", handleRoot);
    server.on("/connect", HTTP_POST, handleConnect);
    server.begin();
    Serial.println("Web Server started");

    Wire.begin();
    lcd.init();
    lcd.backlight();

    WiFiManager wm;
    if (!wm.autoConnect("ESP32_AP")) {
        Serial.println("Failed to connect");
        ESP.restart();
    }

    Serial.println("Connected to WiFi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");

    startDFPlayer();
    menuOptions();
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
    checkWiFiConnection();
    fetchEmergencyLevelOverWiFi();
    handlePinData();
    muteButton();
    handleONOFFButton();



}


void handleONOFFButton() {

    bool reading = digitalRead(ON_OFF_BUTTON_PIN);
    
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if (millis() - lastDebounceTime > debounceDelay && currentButtonState != lastButtonState) {
        lastButtonState = currentButtonState; // Update last state
            if (buttonPressed == LOW) { // Button pressed (active low)
                ledState = !ledState; // Toggle LED state
                digitalWrite(LED_PIN, ledState ? HIGH : LOW);
                Serial.println(ledState ? "KRAKE ON" : "KRAKE OFF");
                
                // Turn off all GPIO pins when the ON/OFF button is pressed
                if (ledState == LOW) {
                    for (int pin = 0; pin <= 39; ++pin) {
                        if (pin != MUTE_BUTTON_PIN && pin != ON_OFF_BUTTON_PIN && pin != LED_PIN) {
                            digitalWrite(pin, LOW);
                        }
                    }
                }
            }
        }
    
        lastButtonState = reading;
      }


void handleRoot() {
    server.send(200, "text/html", htmlPage);
}

void handleConnect() {
    unsigned long startTime = millis();
    int timeout = 30000;  // 30000 seconds timeo
    
    ssid = server.arg("ssid");
    password = server.arg("password");

    server.send(200, "text/html", "<html><body><h1>Connecting...</h1></body></html>");

    WiFi.softAPdisconnect(true);
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeout ) {
        Serial.println("Connecting to WiFi...");
        timeout--;
         delay(100); // Small delay to prevent WDT reset, should ideally handle with other means
    }

    if (WiFi.status() == WL_CONNECTED) {
        String connectedPage = "<html><body><h1>ESP32 Connected to Wi-Fi</h1><p>IP Address: " + WiFi.localIP().toString() + "</p></body></html>";
        server.send(200, "text/html", connectedPage);
        Serial.println("Connected to WiFi");
    } else {
        server.send(200, "text/html", "<html><body><h1>Failed to connect. Please try again.</h1></body></html>");
        Serial.println("Failed to connect to WiFi");
        WiFi.softAP(ssidAP, passwordAP);
    }
}

void muteButton() {  
  //  int currentState = digitalRead(MUTE_BUTTON_PIN);
  //   if (lastState == LOW && currentState == HIGH) {
  //       Serial.println("Mute Button released");
  //       if (trackPlaying) {
          
    static unsigned long lastDebounceTime = 0;
    static bool lastButtonState = HIGH;
    bool currentButtonState = digitalRead(MUTE_BUTTON_PIN);

    if (currentButtonState != lastButtonState) {
        lastDebounceTime = millis();
    }

    if (millis() - lastDebounceTime > debounceDelay && currentButtonState != lastButtonState) {
        lastButtonState = currentButtonState;
        if (currentButtonState == LOW) {
          Serial.println("Mute Button released");
           if (trackPlaying) {
            myDFPlayer.pause();
            trackPlaying = false;
            delay(600000);  // Alarm paused for 10 minutes
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Alarm Paused");

            } else {
            myDFPlayer.start();
            trackPlaying = true;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Alarm Resumed");
            }
        delay(700);
         }
    
    }

}

void handlePinData() {
    int sensorValue = analogRead(analogPin);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Analog Pin Value:");
    lcd.setCursor(0, 1);
    lcd.print(sensorValue);
    Serial.println("Sensor Value: " + String(sensorValue));
    sendSensorValueToHTTPServer(sensorValue);
    sendSensorValueOverWiFi(sensorValue);
    int emergencyLevel = fetchEmergencyLevelOverWiFi();
    Serial.println("Received Emergency Level: " + String(emergencyLevel));
     ;
    handleEmergencyLamps(emergencyLevel);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Emergency level:");
    lcd.setCursor(0, 1);
    lcd.print(emergencyLevel);
}

void sendSensorValueToHTTPServer(int sensorValue) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "/update";
        String postData = "value=" + String(sensorValue);
        http.begin(server_address, serverPort, url);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        int httpCode = http.POST(postData);
        if (httpCode == 200) {
            Serial.println("HTTP POST request successful");
            Serial.println(http.getString());
        } else {
            Serial.println("HTTP POST request failed");
            Serial.println("HTTP Code: " + String(httpCode));
        }
        http.end();
        
    } else {
        Serial.println("WiFi not connected");
    }
}

void sendSensorValueOverWiFi(int value) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        String url = "/update?value=" + String(value);
        if (client.connect(server_address, serverPort)) {
            client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                         "Host: " + server_address + "\r\n" +
                         "Connection: close\r\n\r\n");
             
            while (client.connected()) {
                String line = client.readStringUntil('\n');
                Serial.println(line);
            }
            client.stop();
            Serial.println("Server disconnected");
        } else {
            Serial.println("Connection to server failed");
        }
    } else {
        Serial.println("WiFi not connected");
    }
}

void checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi lost, reconnecting...");
        WiFiManager wm;
        if (!wm.autoConnect("ESP32_AP")) {
            Serial.println("Failed to connect");
            ESP.restart();
        }
    }
}

void startDFPlayer() {

  // Start DFPlayer using hardware serial
    if (!myDFPlayer.begin(mySerial1)) {  // Use mySerial1 instead of mySoftwareSerial
        Serial.println("Unable to begin:");
        Serial.println("1. Please recheck the connection!");
        Serial.println("2. Please insert the SD card!");
        while (true);
    }
    myDFPlayer.setTimeOut(500); // Set serial communictaion time out 500ms
    myDFPlayer.volume(25);  // Set initial volume
    myDFPlayer.EQ(0);  // Normal equalization
    Serial.println("DFPlayer initialized");
}

int fetchEmergencyLevelOverWiFi() {
  WiFiClient client;
  String url = "/emergency";
  if (client.connect(server_address, serverPort)) {
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server_address + "\r\n" +
                 "Connection: close\r\n\r\n");
    
    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.print("read this: ");
      Serial.println(line);
      if (line.startsWith("{\"level\":")) {
        int emergencyLevel = line.substring(9).toInt(); // Adjust the substring index
        Serial.println("we are turning emergency level");
        Serial.write(emergencyLevel);
        return emergencyLevel;
      }
    }
    client.stop();
    Serial.println("Server disconnected");
  } else {
    Serial.println("Connection to server failed");
  }
  return 0; // Return an appropriate default value if the fetch fails
}

void blinkLamp(int lampPin, unsigned long blinkInterval) {
    static unsigned long lastBlinkTime = 0;
    if (millis() - lastBlinkTime >= blinkInterval) {
        int state = digitalRead(lampPin); // Get current state of the lamp
        digitalWrite(lampPin, !state); // Toggle lamp state
        lastBlinkTime = millis();
    }
}

void handleEmergencyLamps(int emergencyLevel) {
  unsigned long currentMillis = millis();
  switch (emergencyLevel) {
    case 1:
      // Blink lamp1 continuously
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        blinkLamp(lampPins[2], 750); // Blink every 750 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
    case 2:
      // Blink lamp2 continuously with faster speed
      if (currentMillis - previousMillis >= interval / 2) {
        previousMillis = currentMillis;
        blinkLamp(lampPins[5], 600); // Blink every 600 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
    case 3:
      // Blink lamp3 continuously with even faster speed
      if (currentMillis - previousMillis >= interval / 4) {
        previousMillis = currentMillis;
        blinkLamp(lampPins[15], 450); // Blink every 450 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
    case 4:
      // Blink lamp4 continuously with even faster speed
      if (currentMillis - previousMillis >= interval / 8) {
        previousMillis = currentMillis;
        blinkLamp(lampPins[18], 300); // Blink every 300 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
    case 5:
      // Blink lamp5 continuously with even faster speed
      if (currentMillis - previousMillis >= interval / 16) {
        previousMillis = currentMillis;
        blinkLamp(lampPins[19], 150); // Blink every 150 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
     default:
      // Turn off all lamps if emergency level is not recognized
      digitalWrite(lampPins[2], LOW);
      digitalWrite(lampPins[5], LOW);
      digitalWrite(lampPins[15], LOW);
      digitalWrite(lampPins[18], LOW);
      digitalWrite(lampPins[19], LOW);
      break;
  }
}


void menuOptions() {
  Serial.println();
  Serial.println("================================================================");
  Serial.println("Commands:");
  Serial.println(" [1-5] Select MP3 file");
  Serial.println(" [p] Pause/resume");
  Serial.println(" [vX] Set volume (0-30)");
  Serial.println(" [+ or -] Increase or decrease volume");
  Serial.println(" [r] Enable repeat mode");
  Serial.println("================================================================");


}

// Function Definitions
// Implementations for handleONOFFButton, handleRoot, handleConnect, muteButton, 
// handlePinData, sendSensorValueToHTTPServer, sendSensorValueOverWiFi, checkWiFiConnection, 
// startDFPlayer, fetchEmergencyLevelOverWiFi, blinkLamp, handleEmergencyLamps, and menuOptions 
// should be updated here, respecting the original logic but formatted for readability.

// NOTE: Detailed implementations of each function are omitted for brevity but should be adapted
// to avoid `delay()` and use non-blocking patterns where necessary.


// delay() are removed 

// krake is not responding to the emergency anymore.