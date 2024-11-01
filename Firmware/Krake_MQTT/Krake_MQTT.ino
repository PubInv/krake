// File name: Krake_MQTT
// Author: Nagham Kheir, Forrest Lee Erickson
// Date: 20241014
// LICENSE "GNU Affero General Public License, version 3 "
// Hardware ESP32 kit
// 20241101 Update for GPAD API and topics "_ALM" and "_ACK"

// Customized this by changing these defines
#define PROG_NAME "Krake_MQTT "
#define VERSION "V0.0.1 "
#define MODEL_NAME "KRAKE_"
#define DEVICE_UNDER_TEST "20240421_USA4"  //A Serial Number  
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "LB"

// Receiver ESP32 Code (Handling Emergency Level 6)

// Description:
// The Receiver ESP32 connects to the MQTT broker and listens for emergency messages from the Sender ESP32.
// When an emergency level 6 message is received, it turns on LEDs connected to GPIO pins (defined for various alerts).
// The code also subscribes to the temperature topic but focuses on responding to emergency level messages.

#include <WiFi.h>
#include <PubSubClient.h> // From library https://github.com/knolleary/pubsubclient

#define BAUDRATE 115200

// WiFi credentials
//const char* ssid = "ADT";
//const char* password = "adt@12345";

//Maryville network
const char* ssid = "VRX";
const char* password = "textinsert";


// MQTT Broker
const char* mqtt_server = "public.cloud.shiftr.io";
const char* mqtt_user = "public";
const char* mqtt_password = "public";

// MQTT Topics
//const char* temperature_topic = "esp32/temperature";
//const char* emergency_topic = "esp32/emergency";  // Topic for emergency messages

const char* subscribe_Alarm_Topic = "KRAKE_20240421_USA4_ALM";
const char* publish_Ack_Topic = "KRAKE_20240421_USA4_ACK";

// LED Pins
// To Do Define ESP32 pins for actual Krake LEDs
// Homework2 Assembly, akd PMD LAMPS aka Other LEDs
const int LED_D9 = 23;  // Mute1 LED on PMD
const int LAMP1 = 15;   // D5 cold food
const int LAMP2 = 4;    // D3 baby crying
const int LAMP3 = 5;    // D8 high BP1
const int LAMP4 = 18;   // D7 shield failure
const int LAMP5 = 19;   // D6 lost sock

// Initialize WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(BAUDRATE);
  delay(500);
  //Serial splash
  Serial.println(F("==================================="));
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.println(MODEL_NAME);
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print("Alarm Topic: ");
  Serial.println(subscribe_Alarm_Topic);
  Serial.print(F("Compiled at: "));
  Serial.println(F(__DATE__ " " __TIME__));  //compile date that is used for a unique identifier
  Serial.println(LICENSE);
  Serial.println(F("==================================="));
  Serial.println();

  // Set LED pins as outputs
  pinMode(LED_D9, OUTPUT);
  pinMode(LAMP1, OUTPUT);
  pinMode(LAMP2, OUTPUT);
  pinMode(LAMP3, OUTPUT);
  pinMode(LAMP4, OUTPUT);
  pinMode(LAMP5, OUTPUT);

  // Turn off all LEDs initially
  turnOnAllLamps();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

// Handeler for MQTT subscrived messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Handle emergency messages
  //  if (String(topic) == emergency_topic) {
  //    int emergencyLevel = message.toInt();
  //    if (emergencyLevel == 6) {
  //      Serial.println("Emergency Level 6 received! Turning on all LEDs.");
  //      turnOnAllLamps();  // Turn on all the lamps
  //    }
  //  }
}

void setup_wifi() {
  delay(10);
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
    if (client.connect("ESP32_Receiver", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      //      client.subscribe(temperature_topic);  // Subscribe to temperature topic
      //      client.subscribe(emergency_topic);    // Subscribe to emergency topic
      client.subscribe(subscribe_Alarm_Topic);    // Subscribe to GPAD API alarms
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

// Function to turn on all lamps
void turnOnAllLamps() {
  digitalWrite(LED_D9, HIGH);  // Turn on Mute1 LED
}
