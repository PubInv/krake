
#include <WiFi.h>
#include <HTTPClient.h>
#include <DFRobotDFPlayerMini.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

WiFiServer server(80);
const char *ssid = "ADT";
const char *password = "adt@12345";
const char *server_address = "192.168.1.3";
const int serverPort = 5500;
const int analogPin = 34;
const int BUTTON_PIN = 36;
const int ON_OFF_PIN = 36; // On/Off button pin
const int lamp1Pin = 2;
const int lamp2Pin = 4;
const int lamp3Pin = 5;
const int lamp4Pin = 18;
const int lamp5Pin = 19;

HardwareSerial myHardwareSerial(2);  // Use UART2 (pins 16 and 17) for DFPlayer
DFRobotDFPlayerMini myDFPlayer;
bool trackPlaying = false;
String line;
char command;
unsigned long previousMillis = 0;
const long interval = 100;

LiquidCrystal_I2C lcd(0x3F, 20, 4);  // Address 0x3F, 20 columns, 4 rows
int lastState = LOW; // the previous state (is low) from the input pin
int currentState;     // the current reading from the input pin
bool deviceOn = true; // Variable to keep track of the device state (on/off)

byte WiFix15[] = { B00000, B11111, B00000, B11111, B00000, B01110, B00000, B00100 };

byte noWiFix15[] = { B00100, B11111, B00100, B11111, B00100, B01110, B00000, B00100 };

  
void setup() {
  Serial.begin(115200);
  myHardwareSerial.begin(9600, SERIAL_8N1, 16, 17);  // DFPlayer serial communication
  Wire.begin();  // Initialize I2C communication
  lcd.init();  // Initialize LCD
  lcd.backlight();  // Turn on LCD backlight
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Initialize button pin
  pinMode(ON_OFF_PIN, INPUT_PULLUP);  // Initialize on/off button pin
  // Initialize lamp pins
  pinMode(lamp1Pin, OUTPUT);
  pinMode(lamp2Pin, OUTPUT);
  pinMode(lamp3Pin, OUTPUT);
  pinMode(lamp4Pin, OUTPUT);
  pinMode(lamp5Pin, OUTPUT);

  // Connect to WiFi and start DF player
  connectToWiFi();
  startDFPlayer();

  // Start server
  server.begin();

  // Check if the module is responding and if the SD card is found
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Initializing DFPlayer module ... Wait!"));
  if (!myDFPlayer.begin(myHardwareSerial)) {
    Serial.println(F("Not initialized:"));
    Serial.println(F("1. Check the DFPlayer Mini connections"));
    Serial.println(F("2. Insert an SD card"));
    while (true);
  }
  Serial.println();
  Serial.println(F("DFPlayer Mini module initialized!"));

  // Initial settings
  myDFPlayer.setTimeOut(500);  // Serial timeout 500ms
  myDFPlayer.volume(25);        // Volume 5
  myDFPlayer.EQ(0);            // Normal equalization
  menuOptions();  // Display menu options on serial monitor


// wifi connected char 
  lcd.createChar(0, WiFix15);
  lcd.setCursor(15, 4);
  lcd.write(0);

// wifi not connected char
  lcd.createChar(0, noWiFix15);
  lcd.setCursor(15, 4);
  lcd.write(0);

}

void loop() {
  // checkOnOffButton(); // Check the on/off button state

  // if (deviceOn) {
    checkWiFiConnection(); // Monitor WiFi connection
    fetchEmergencyLevelOverWiFi();
    handleWiFiClientRequests();
    handlePinData();
    muteButton();
    
    // Pause or resume
    // read the state of the switch/button:
    currentState = digitalRead(BUTTON_PIN);
    // Check if the button state changed from LOW to HIGH (button released)
    if (lastState == LOW && currentState == HIGH) {
      Serial.println("Button released");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alarm Paused");
      if (trackPlaying) {
        myDFPlayer.pause();
        trackPlaying = false;
        // Alarm paused for 10 minutes
        delay(600000); // 10 minutes delay
        Serial.println("Alarm Paused.");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Alarm Paused");
      } else {
        myDFPlayer.start();
        trackPlaying = true;
        Serial.println("Resumed.");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Alarm Resumed");
      }
    }

    // Save the last state
    lastState = currentState;
  }


// void checkOnOffButton() {
//   int buttonState = digitalRead(ON_OFF_PIN);

//   if (buttonState == LOW) { // Assuming LOW means button is pressed
//     deviceOn = !deviceOn; // Toggle device state
//     delay(500); // Debounce delay
//     if (deviceOn) {
//       lcd.clear();
//       lcd.setCursor(0, 0);
//       lcd.print("Device ON");
//       Serial.println("Device ON");
//     } else {
//       lcd.clear();
//       lcd.setCursor(0, 0);
//       lcd.print("Device OFF");
//       Serial.println("Device OFF");
//     }
//   }
// }

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    lcd.createChar(0, WiFix15);
    connectToWiFi();
  }
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(700);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.createChar(0, noWiFix15);
}

void muteButton() {
  int lastState = LOW; 
  int currentState;
  // read the state of the switch/button:
  currentState = digitalRead(BUTTON_PIN);
  // Check if the button state changed from LOW to HIGH (button released)
  if (lastState == LOW && currentState == HIGH) {
    Serial.println("Button released");
    if (trackPlaying) {
      myDFPlayer.pause();
      trackPlaying = false;
      // Allarm paused for 10 minutes
      delay(600000); // 10 minutes delay
      Serial.println("ALLARM Paused.");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alarm Paused");
    } else {
      myDFPlayer.start();
      trackPlaying = true;
      Serial.println("Resumed.");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alarm Resumed");
    }
    delay(700);
  }
  // save the last state
  lastState = currentState;
}

void handleWiFiClientRequests() {
  WiFiClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print("Received from client: ");
        Serial.println(line);
      }
    }
    client.stop();
    Serial.println("WiFi client disconnected");
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
  // Send the sensor value to the HTTP server
  sendSensorValueToHTTPServer(sensorValue);
  // Send the sensor value over WiFi
  sendSensorValueOverWiFi(sensorValue);
  // Fetch emergency level over WiFi
  int emergencyLevel = fetchEmergencyLevelOverWiFi();
  Serial.println("Received Emergency Level: " + String(emergencyLevel));
  delay(700);
  // Handle emergency lamps based on the received level
  handleEmergencyLamps(emergencyLevel);
  // Print the Emergency level on the LCD 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Emergency level:");
  lcd.setCursor(0, 1);
  lcd.print(emergencyLevel);
}

void sendSensorValueToHTTPServer(int sensorValue) {
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
}

void sendSensorValueOverWiFi(int value) {
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
}


void startDFPlayer() {
  if (!myDFPlayer.begin(myHardwareSerial)) {
    Serial.println("Unable to begin DFPlayer");
    while (true);
  }
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(25);  // Set initial volume
  myDFPlayer.EQ(0);  // Normal equalization
  Serial.println("DFPlayer initialized");
}

int fetchEmergencyLevelOverWiFi(){
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

