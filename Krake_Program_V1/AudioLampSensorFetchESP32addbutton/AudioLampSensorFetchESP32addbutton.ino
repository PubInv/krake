#include <WiFi.h>
#include <HTTPClient.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
// #include "SoftwareSerial.h"
// #include "DFRobotDFPlayerMini.h"


WiFiServer server(80);
const char *ssid = "ADT";
const char *password = "adt@12345";
const char *server_address = "192.168.1.3";
const int serverPort = 5500;
const int analogPin = 34;
const int lamp1Pin = 2;
const int lamp2Pin = 4;
const int lamp3Pin = 5;
const int lamp4Pin = 18;
const int lamp5Pin = 19;

const int BUTTON_PIN = 36; // GPIO36 pin connected to button

// Variables will change:
int lastState = LOW; // the previous state (is low) from the input pin
int currentState;     // the current reading from the input pin

// Initialize software serial on pins 16 and 17
SoftwareSerial mySoftwareSerial(16, 17);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;
String line;
char command;
bool trackPlaying = false;


unsigned long previousMillis = 0;
const long interval = 100; // Adjust the interval based on your needs

// Create the Player object
DFRobotDFPlayerMini player;

void setup() {

 //Serial communication with the module
  mySoftwareSerial.begin(9600);
  // Initialize Arduino serial
  Serial.begin(115200);

 // initialize the pushbutton pin as an pull-up input
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  pinMode(lamp1Pin, OUTPUT);
  pinMode(lamp2Pin, OUTPUT);
  pinMode(lamp3Pin, OUTPUT);
  pinMode(lamp4Pin, OUTPUT);
  pinMode(lamp5Pin, OUTPUT);

  // Connect to WiFi
  connectToWiFi();

  // Start server
  server.begin();

 
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
  myDFPlayer.volume(25);        // Volume 5
  myDFPlayer.EQ(0);            // Normal equalization

  menu_options();


  // read the state of the switch/button:
  currentState = digitalRead(BUTTON_PIN);


}

void loop() {

  handleWiFiClientRequests();
  handlePinData();
  fetchEmergencyLevelOverWiFi();
  muteButton();
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
    if (lastState == LOW && currentState == HIGH) {
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

void muteButton () {
 
 
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
    } else {
        myDFPlayer.start();
        trackPlaying = true;
        Serial.println("Resumed.");
      }
    
    delay (700);
  }
   
   // save the last state
  lastState = currentState;


}

void blinkLamp(int lampPin, int blinkDelay) {
  digitalWrite(lampPin, HIGH);
  delay(blinkDelay / 2); // On for half of the blink delay
  digitalWrite(lampPin, LOW);
  delay(blinkDelay / 2); // Off for the other half of the blink delay
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(700);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void handleWiFiClientRequests() {
  WiFiClient client = server.available();
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
  delay(700);

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