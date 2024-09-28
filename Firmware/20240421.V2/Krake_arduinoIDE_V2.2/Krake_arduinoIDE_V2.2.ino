#include <WiFi.h>
#include <HTTPClient.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
// #include "SoftwareSerial.h"
// #include "DFRobotDFPlayerMini.h"


WiFiServer server(80);
const char *ssid = "ADT";
const char *password = "adt@12345";
const char *server_address = "192.168.1.9";
const int serverPort = 5500;

// Configuration and Pin Definitions
const int analogPin = 34;
const int MUTE_BUTTON_PIN = 36;
const int ON_OFF_BUTTON_PIN = 39;
const int LED_PIN = 2;                        // on off LED
const int LED_PIN_M[] = { 23 };               // Mute LED
const int lampPins[] = { 15, 4, 5, 18, 19 };  // Emergency lamp pins including MUTE LED


// Initialize software serial on pins 16 and 17
SoftwareSerial mySoftwareSerial(16, 17);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;
String line;
char command;
bool trackPlaying = false;


unsigned long previousMillis = 0;
const long interval = 100;  // Adjust the interval based on your needs

// Create the Player object
DFRobotDFPlayerMini player;

void setup() {

  // Serial communication with the module
  mySoftwareSerial.begin(9600);

  Serial.begin(115200);



  pinMode(MUTE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ON_OFF_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  for (int pin : lampPins) {
    pinMode(pin, OUTPUT);
  }

  // Connect to WiFi
  connectToWiFi();

  // Start server
  server.begin();

  //Serial communication with the module
  mySoftwareSerial.begin(9600);
  // Initialize Arduino serial
  Serial.begin(115200);
  // Check if the module is responding and if the SD card is found
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Initializing DFPlayer module ... Wait!"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Not initialized:"));
    Serial.println(F("1. Check the DFPlayer Mini connections"));
    Serial.println(F("2. Insert an SD card"));
    while (true)
      ;
  }
  Serial.println();
  Serial.println(F("DFPlayer Mini module initialized!"));
  // Initial settings
  myDFPlayer.setTimeOut(500);  // Serial timeout 500ms
  myDFPlayer.volume(25);       // Volume 5
  myDFPlayer.EQ(0);            // Normal equalization

  menu_options();
}

void loop() {

  handleWiFiClientRequests();
  handlePinData();
  fetchEmergencyLevelOverWiFi();
  // handleEmergencyLamps();


  // Waits for data entry via serial
  while (Serial.available() > 0) {
    command = Serial.read();

    // Play from first 9 files
    if ((command >= '1') && (command <= '5')) {
      Serial.print("Music reproduction: ");
      Serial.println(command);
      command = command - '0';
      myDFPlayer.play(command);
      myDFPlayer.enableLoop();
      trackPlaying = true;
      menu_options();
    }

    // // Toggle repeat mode
    // if (command == 'r') {
    //   myDFPlayer.enableLoop();
    //   Serial.println("Repeat mode enabled.");
    // }

    // Pause or resume
    if (command == 'p') {
      if (trackPlaying) {
        myDFPlayer.pause();
        trackPlaying = false;
        Serial.println("Paused. Press 'p' again to resume or select another track.");
      } else {
        myDFPlayer.start();
        trackPlaying = true;
        Serial.println("Resumed.");
      }
      menu_options();
    }

    // Set volume
    if (command == 'v') {
      if (Serial.available() > 0) {
        int myVolume = Serial.parseInt();
        if (myVolume >= 0 && myVolume <= 30) {
          myDFPlayer.volume(myVolume);
          Serial.print("Current Volume: ");
          Serial.println(myDFPlayer.readVolume());
        } else {
          Serial.println("Invalid volume level, choose a number between 0-30.");
        }
      }
      menu_options();
    }

    // Increases volume
    if (command == '+') {
      myDFPlayer.volumeUp();
      Serial.print("Current Volume: ");
      Serial.println(myDFPlayer.readVolume());
      menu_options();
    }

    // Decreases volume
    if (command == '-') {
      myDFPlayer.volumeDown();
      Serial.print("Current Volume: ");
      Serial.println(myDFPlayer.readVolume());
      menu_options();
    }
  }

  // Check if the track is still playing and restart if it's not
  if (trackPlaying && !myDFPlayer.available()) {
    myDFPlayer.play();
  }
}

void blinkLamp(int lampPin, int blinkDelay) {
  digitalWrite(lampPin, HIGH);
  delay(blinkDelay / 2);  // On for half of the blink delay
  digitalWrite(lampPin, LOW);
  delay(blinkDelay / 2);  // Off for the other half of the blink delay
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void handleWiFiClientRequests() {
  WiFiClient client =
    server.available();
  if (client) {
    Serial.println("New WiFi client");

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
  Serial.println("Sensor Value: " + String(sensorValue));

  // Send the sensor value to the HTTP server
  sendSensorValueToHTTPServer(sensorValue);

  // Send the sensor value over WiFi
  sendSensorValueOverWiFi(sensorValue);

  // Fetch emergency level over WiFi
  int emergencyLevel = fetchEmergencyLevelOverWiFi();
  Serial.println("Received Emergency Level: " + String(emergencyLevel));
  delay(1000);

  // Handle emergency lamps based on the received level
  handleEmergencyLamps(emergencyLevel);
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
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + server_address + "\r\n" + "Connection: close\r\n\r\n");

    delay(1000);

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


int fetchEmergencyLevelOverWiFi() {
  WiFiClient client;
  String url = "/emergency";

  if (client.connect(server_address, serverPort)) {
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + server_address + "\r\n" + "Connection: close\r\n\r\n");

    delay(1000);

    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.print("testing line 289 :");
      Serial.print("read this: ");
      Serial.println(line);
      // if (line.startsWith("{\"level\":"))
      {
        int emergencyLevel = line.substring(160).toInt();  // Adjust the substring index
        Serial.print("testing line 291 :");
        Serial.println(line.substring(160).toInt());
        //   Serial.println("we are turning emergency level");
        Serial.write(emergencyLevel);
        return emergencyLevel;
      }
    }

    client.stop();
    Serial.println("Server disconnected");
  } else {
    Serial.println("Connection to server failed");
  }

  return 0;  // Return an appropriate default value if the fetch fails
}

void handleEmergencyLamps(int emergencyLevel) {
  unsigned long currentMillis = millis();
  switch (emergencyLevel) {
    case 0:
      // Turn off all lamps if emergency level is not recognized
      blinkLamp(lampPins[23], 750);
      digitalWrite(lampPins[5], LOW);
      digitalWrite(lampPins[15], LOW);
      digitalWrite(lampPins[18], LOW);
      digitalWrite(lampPins[19], LOW);
      break;
    case 1:
      // Blink lamp1 continuously
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        blinkLamp(lampPins[4], 750);  // Blink every 750 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
    case 2:
      // Blink lamp2 continuously with faster speed
      if (currentMillis - previousMillis >= interval / 2) {
        previousMillis = currentMillis;
        blinkLamp(lampPins[5], 600);  // Blink every 600 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
    case 3:
      // Blink lamp3 continuously with even faster speed
      if (currentMillis - previousMillis >= interval / 4) {
        previousMillis = currentMillis;
        blinkLamp(lampPins[15], 450);  // Blink every 450 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
    case 4:
      // Blink lamp4 continuously with even faster speed
      if (currentMillis - previousMillis >= interval / 8) {
        previousMillis = currentMillis;
        blinkLamp(lampPins[18], 300);  // Blink every 300 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
    case 5:
      // Blink lamp5 continuously with even faster speed
      if (currentMillis - previousMillis >= interval / 16) {
        previousMillis = currentMillis;
        blinkLamp(lampPins[19], 150);  // Blink every 150 milliseconds
        myDFPlayer.play(emergencyLevel);
        myDFPlayer.enableLoop();
      }
      break;
    default:
      Serial.println("INTERNAL PROGRAMMING ERROR: EMERGENCY LEVEL TOO HIGH!");
      break;
  }
}

void menu_options() {
  Serial.println();
  Serial.println(F("=================================================================================================================================="));
  Serial.println(F("Commands:"));
  Serial.println(F(" [1-5] To select the MP3 file"));
  Serial.println(F(" [p] pause/resume"));
  Serial.println(F(" [vX] set volume to X"));
  Serial.println(F(" [+ or -] increases or decreases the volume"));
  Serial.println(F(" [r] enable repeat mode"));
  Serial.println();
  Serial.println(F("================================================================================================================================="));
}