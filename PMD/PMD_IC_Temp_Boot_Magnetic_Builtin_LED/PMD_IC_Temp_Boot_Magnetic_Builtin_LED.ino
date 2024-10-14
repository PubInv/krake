// Sender ESP32 Code (Sending Emergency Level 6 on BOOT Button Press)

// Description:
// The Sender ESP32 connects to WiFi and an MQTT broker to communicate with the Receiver ESP32.
// It monitors the internal temperature and sends this value to the MQTT broker every 5 seconds.
// When the BOOT button (GPIO 0) is pressed, it sends an "emergency level 6" message to the Receiver ESP32 via MQTT.
// The internal LED (GPIO 2) blinks to indicate a successful connection with the Receiver ESP32.


#include "esp_system.h"  // ESP32 system functions
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "ADT";
const char* password = "adt@12345";

// MQTT Broker
const char* mqtt_server = "public.cloud.shiftr.io";
const char* mqtt_user = "public";
const char* mqtt_password = "public";

// MQTT Topics
const char* temperature_topic = "esp32/temperature";
const char* connection_confirmation_topic = "esp32/confirmation";
const char* emergency_topic = "esp32/emergency";  // Topic for emergency messages

// Internal LED pin
const int LED_PIN = 2;
const int BOOT_BUTTON_PIN = 0;  // BOOT button is connected to GPIO 0

// Define alarm threshold temperature in degrees Celsius
const float TEMPERATURE_THRESHOLD = 50.0;

// Initialize WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// Variables for timing
unsigned long previousTempMillis = 0;  // For sending temperature every 5 seconds
unsigned long tempInterval = 5000;     // 5 seconds interval for temperature check
unsigned long previousBlinkMillis = 0; // For blinking the LED
unsigned long blinkInterval = 500;     // 500 ms interval for LED blinking
bool isReceiverConnected = false;      // Receiver connection flag
int blinkCount = 0;                    // To track LED blink cycles

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);  // Set LED pin as output
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);  // Set BOOT button as input with pull-up resistor
  digitalWrite(LED_PIN, LOW);  // Start with LED off
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);  // Set callback function for MQTT
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Check if BOOT button is pressed
  if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
    sendEmergencyLevel6();  // Send emergency level 6 when button is pressed
    delay(500);  // Simple debounce delay to avoid multiple sends on a single press
  }

  // Send temperature every 5 seconds using millis()
  unsigned long currentMillis = millis();
  if (isReceiverConnected && (currentMillis - previousTempMillis >= tempInterval)) {
    previousTempMillis = currentMillis;
    sendTemperature();
  }

  // If receiver is connected, blink the LED three times
  if (isReceiverConnected && blinkCount < 6) {  // Blink LED three times (HIGH and LOW count as one blink)
    if (currentMillis - previousBlinkMillis >= blinkInterval) {
      previousBlinkMillis = currentMillis;
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));  // Toggle LED state
      blinkCount++;
    }
  }
}

void setup_wifi() {
  delay(10);  // This can be kept as WiFi setup isn't critical for non-blocking
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_Sender", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(connection_confirmation_topic);  // Subscribe to confirmation topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void sendTemperature() {
  float temp = temperatureRead();
  // char tempString[8];
  // dtostrf(temp, 1, 2, tempString);  // Convert float to string
 //const char = 25 
 #define tempString "the temp in LEB os 20c" 
  // Publish temperature to MQTT
  client.publish(temperature_topic, tempString);
  Serial.print("Temperature sent: ");
  Serial.println(tempString);
}

void sendEmergencyLevel6() {
  // Send emergency level 6 to the receiver
  client.publish(emergency_topic, "6");
  Serial.println("Emergency level 6 sent!");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String receivedMessage = "";
  for (int i = 0; i < length; i++) {
    receivedMessage += (char)payload[i];
  }
  Serial.println(receivedMessage);

  // Check if the message is from the confirmation topic
  if (String(topic) == connection_confirmation_topic && receivedMessage == "connected") {
    Serial.println("Receiver connected, starting to blink LED");
    isReceiverConnected = true;
    blinkCount = 0;  // Reset blink counter to start blinking
  }
}
