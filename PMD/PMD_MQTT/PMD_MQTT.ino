#include <WiFi.h>
#include <PubSubClient.h>
#include <esp_wifi.h>

// MQTT Broker
const char* mqtt_server = "public.cloud.shiftr.io";
const char* mqtt_user = "public";
const char* mqtt_password = "public";

// MQTT Topics
char subscribe_Alarm_Topic[17];
char publish_Ack_Topic[17];
char macAddressString[13];

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MACSTR_PLN "%02X%02X%02X%02X%02X%02X"

// WiFi and MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);

// Publish an online message every 10 seconds
void publishOnLineMsg() {
  const unsigned long MESSAGE_PERIOD = 10000;
  static unsigned long lastMillis = 0;
  if ((millis() - lastMillis > MESSAGE_PERIOD) || (millis() < lastMillis)) { 
    lastMillis = millis();
    Serial.println("Publishing online status...");
    client.publish(publish_Ack_Topic, "ESP32 Online");
  }
}

// Reconnect to MQTT broker
void reconnect() {
  int attempts = 0;
  while (!client.connected() && attempts < 5) {
    attempts++;
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_Receiver", mqtt_user, mqtt_password)) {
      Serial.println("Connected!");
      client.subscribe(subscribe_Alarm_Topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      delay(1000);
    }
  }
  Serial.println(client.connected() ? "MQTT Reconnected!" : "MQTT Reconnect Failed!");
}


// MQTT Message Callback
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received Topic: ");
  Serial.println(topic);

  char msgBuffer[121];
int msgLength = min(length, (unsigned int)120);
  for (int i = 0; i < msgLength; i++) {
    msgBuffer[i] = (char)payload[i];
  }
  msgBuffer[msgLength] = '\0';

  Serial.print("Payload: ");
  Serial.println(msgBuffer);
}

// Read MAC Address
bool readMacAddress(uint8_t* baseMac) {
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    return true;
  } else {
    Serial.println("Failed to read MAC address");
    return false;
  }
}

// Setup Function
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  uint8_t mac[6];
  readMacAddress(mac);
  sprintf(macAddressString, MACSTR_PLN, MAC2STR(mac));

  strcpy(subscribe_Alarm_Topic, macAddressString);
  strcpy(publish_Ack_Topic, macAddressString);
  strcat(subscribe_Alarm_Topic, "_ALM");
  strcat(publish_Ack_Topic, "_ACK");

  Serial.println("MQTT Topics:");
  Serial.println(subscribe_Alarm_Topic);
  Serial.println(publish_Ack_Topic);

  reconnect();
}

// Main Loop
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  publishOnLineMsg();
  delay(100);
}
