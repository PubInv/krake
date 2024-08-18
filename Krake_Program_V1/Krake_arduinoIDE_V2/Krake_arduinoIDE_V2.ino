#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <HTTPClient.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFiManager.h>

const char* server_address = "192.168.1.3";
const int serverPort = 5500;
const int analogPin = 34;
const int MUTE_BUTTON_PIN = 36; // GPIO 36 for the mute button
const int ON_OFF_BUTTON_PIN = 39; // GPIO 39 for the ON/OFF button

#define LED_PIN 2 // GPIO 2 for the blue LED // ON OFF LED
const int lamp1Pin = 15; // lamp emergency L1
const int lamp2Pin = 4; //lamp emergency L2
const int lamp3Pin = 5; //lamp emergency L3
const int lamp4Pin = 18;//lamp emergency L4
const int lamp5Pin = 19;//lamp emergency L5
const int lamp6Pin = 23; // MUTE LED 

const char* ssidAP = "ESP32-Access-Point";
const char* passwordAP = "123456789";
const byte DNS_PORT = 53;

bool ledState = false; // Initial state of the LED
unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 50; // Debounce time in milliseconds
bool lastButtonState = HIGH;
bool currentButtonState;
bool buttonPressed = false;

SoftwareSerial mySoftwareSerial(16, 17);  // RX, TX for DFPlayer
DFRobotDFPlayerMini myDFPlayer;
LiquidCrystal_I2C lcd(0x3F, 20, 4);  // Address 0x3F, 20 columns, 4 rows
WebServer server(80);
DNSServer dnsServer;

String ssid = "";
String password = "";
bool trackPlaying = false;
int lastState = LOW;
unsigned long previousMillis = 0;
const long interval = 100;

const char* htmlPage = R"rawliteral(
<html>
<body>
<h1>Enter Wi-Fi Credentials</h1>
<form action="/connect" method="POST">
SSID:<br>
<input type="text" name="ssid"><br>
Password:<br>
<input type="password" name="password"><br><br>
<input type="submit" value="Submit">
</form>
</body>
</html>)rawliteral";

void setup() {
    Serial.begin(115200);
    pinMode(MUTE_BUTTON_PIN, INPUT_PULLUP); // Set mute button pin as input with pull-up
    pinMode(ON_OFF_BUTTON_PIN, INPUT_PULLUP); // Set ON/OFF button pin as input with pull-up
    pinMode(LED_PIN, OUTPUT);          // Set LED pin as output
    digitalWrite(LED_PIN, LOW);        // Initialize LED to off

    // Initialize lamp pins
    pinMode(lamp1Pin, OUTPUT);
    pinMode(lamp2Pin, OUTPUT);
    pinMode(lamp3Pin, OUTPUT);
    pinMode(lamp4Pin, OUTPUT);
    pinMode(lamp5Pin, OUTPUT);

    // Set up as Access Point
    WiFi.softAP(ssidAP, passwordAP);
    Serial.println("Access Point Created");

    // Set up DNS server to redirect all URLs to the web server
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

    // Set up web server
    server.on("/", handleRoot);
    server.on("/connect", HTTP_POST, handleConnect);
    server.begin();
    Serial.println("Web Server started");

    // Initialize peripherals
    mySoftwareSerial.begin(9600);
    Wire.begin();
    lcd.init();
    lcd.backlight();

    // Initialize WiFiManager
    WiFiManager wm;
    if (!wm.autoConnect("ESP32_AP")) {
        Serial.println("Failed to connect");
        ESP.restart();
    }

    // Connected to WiFi
    Serial.println("Connected to WiFi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");

    // Start DF player
    startDFPlayer();
    menuOptions();
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
    checkWiFiConnection();
    fetchEmergencyLevelOverWiFi();
    // handleWiFiClientRequests();
    handlePinData();
    muteButton();
    handleONOFFButton();

    }

void handleONOFFButton() {

    bool reading = digitalRead(ON_OFF_BUTTON_PIN);
    
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonPressed) {
            buttonPressed = reading;
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
    }
    lastButtonState = reading;
}


void handleRoot() {
    server.send(200, "text/html", htmlPage);
}

void handleConnect() {
    ssid = server.arg("ssid");
    password = server.arg("password");

    server.send(200, "text/html", "<html><body><h1>Connecting...</h1></body></html>");

    WiFi.softAPdisconnect(true);
    WiFi.begin(ssid.c_str(), password.c_str());

    int timeout = 30;  // 30 seconds timeout
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
        timeout--;
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
    int currentState = digitalRead(MUTE_BUTTON_PIN);
    if (lastState == LOW && currentState == HIGH) {
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
    lastState = currentState;
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
    delay(700);
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
        delay(1000);
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
            delay(700);
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
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Unable to begin DFPlayer");
    while (true);
  }
  myDFPlayer.setTimeOut(500);
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
    delay(700);
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

void blinkLamp(int lampPin, int blinkDelay) {
  digitalWrite(lampPin, HIGH);
  delay(blinkDelay / 2); // On for half of the blink delay
  digitalWrite(lampPin, LOW);
  delay(blinkDelay / 2); // Off for the other half of the blink delay
}

void handleEmergencyLamps(int emergencyLevel) {
  unsigned long currentMillis = millis();
  switch (emergencyLevel) {
    case 1:
      // Blink lamp1 continuously
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        blinkLamp(lamp1Pin, 750); // Blink every 750 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
    case 2:
      // Blink lamp2 continuously with faster speed
      if (currentMillis - previousMillis >= interval / 2) {
        previousMillis = currentMillis;
        blinkLamp(lamp2Pin, 600); // Blink every 600 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
    case 3:
      // Blink lamp3 continuously with even faster speed
      if (currentMillis - previousMillis >= interval / 4) {
        previousMillis = currentMillis;
        blinkLamp(lamp3Pin, 450); // Blink every 450 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
    case 4:
      // Blink lamp4 continuously with even faster speed
      if (currentMillis - previousMillis >= interval / 8) {
        previousMillis = currentMillis;
        blinkLamp(lamp4Pin, 300); // Blink every 300 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
    case 5:
      // Blink lamp5 continuously with even faster speed
      if (currentMillis - previousMillis >= interval / 16) {
        previousMillis = currentMillis;
        blinkLamp(lamp5Pin, 150); // Blink every 150 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
     default:
      // Turn off all lamps if emergency level is not recognized
      digitalWrite(lamp1Pin, LOW);
      digitalWrite(lamp2Pin, LOW);
      digitalWrite(lamp3Pin, LOW);
      digitalWrite(lamp4Pin, LOW);
      digitalWrite(lamp5Pin, LOW);
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
