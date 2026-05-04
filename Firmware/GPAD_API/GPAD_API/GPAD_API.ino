/* GPAD_API.ino
  The program implements the main API of the General Purpose Alarm Device.

  Copyright (C) 2025 Robert Read

  This program includes free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  See the GNU Affero General Public License for more details.
  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  Change notes:
  20221024 Creation by Rob.
  20221028 Report Program Name in splash screen. Lee Erickson
*/

/* This is a work-in-progress but it has two purposes.
 * It essentially implements two APIs: An "abstract" API that is
 * intended to be unchanging and possibly implemented on a large
 * variety of hardware devices. That is, as the GPAD hardware
 * changes and evolves, it does not invalidate the use of this API.
 *
 * Secondly, it offers a fully robotic API; that is, it gives
 * complete access to all of the hardware currently on the board.
 * For example, the current hardware, labeled Prototype #1, offers
 * a simple "tone" buzzer. The abstract interface uses this as part
 * of an abstract command like "set alarm level to PANIC".
 * The robotic control allows you to specify the actual tone to be played.
 * The robotic inteface obviously changes as the hardware changes.
 *
 * Both APIs are useful in different situations. The abstract interface
 * is good for a medical device manufacturer that expects the alarming
 * device to change and evolve. The Robotic API is good for testing the
 * actual hardware, and for a hobbyist that wants to use the device for
 * more than simple alarms, such as for implementing a game.
 *
 * It is our intention that the API will be available both through the
 * serial port and through an SPI interface. Again, these two modes
 * serve different purposes. The SPI interface is good for tight
 * intergration with a safey critical devices. The serial port approach
 * is easier for testing and for a hobbyist to develop an approach,
 * whether they eventually intend to use the SPI interface or not.
 * -- rlr, Oct. 24, 2022
 */

#include "alarm_api.h"
#include "GPAD_HAL.h"
#include "gpad_utility.h"
#include "gpad_serial.h"
#include "Wink.h"
#include <math.h>

#include <WiFi.h>
#include <esp_wifi.h>

#include <PubSubClient.h> // From library https://github.com/knolleary/

#include <WiFiManager.h> // WiFi Manager for ESP32
#include <LittleFS.h>
#include <ElegantOTA.h>
#include <FS.h>   // File System Support
#include <Wire.h> // req for i2c comm

#include "WiFiManagerOTA.h"
#include <ESPAsyncWebServer.h>
#include <string.h>

#include "InterruptRotator.h"

#include "DFPlayer.h"
#include "GPAD_menu.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const size_t SERIAL_LOG_MAX_CHARS = 12000;
String serialLogBuffer;

void appendSerialLog(const char *text, size_t len)
{
  if (len == 0)
  {
    return;
  }

  if (len > SERIAL_LOG_MAX_CHARS)
  {
    text += (len - SERIAL_LOG_MAX_CHARS);
    len = SERIAL_LOG_MAX_CHARS;
  }

  for (size_t i = 0; i < len; ++i)
  {
    serialLogBuffer += text[i];
  }
  if (serialLogBuffer.length() > SERIAL_LOG_MAX_CHARS)
  {
    serialLogBuffer.remove(0, serialLogBuffer.length() - SERIAL_LOG_MAX_CHARS);
  }
}

class SerialMirrorStream : public Stream
{
public:
  explicit SerialMirrorStream(HardwareSerial &serialPort)
      : port(serialPort) {}

  void begin(unsigned long baud) { port.begin(baud); }
  void setTimeout(unsigned long timeoutMs) { port.setTimeout(timeoutMs); }
  operator bool() const { return static_cast<bool>(port); }

  int available() override { return port.available(); }
  int read() override { return port.read(); }
  int peek() override { return port.peek(); }
  void flush() override { port.flush(); }

  size_t write(uint8_t ch) override
  {
    const char c = static_cast<char>(ch);
    appendSerialLog(&c, 1);
    return port.write(ch);
  }

  size_t write(const uint8_t *buffer, size_t size) override
  {
    appendSerialLog(reinterpret_cast<const char *>(buffer), size);
    return port.write(buffer, size);
  }

private:
  HardwareSerial &port;
};

SerialMirrorStream debugSerial(Serial);

// Initialize WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

WifiOTA::Manager wifiManager(WiFi, debugSerial);

/* SPI_PERIPHERAL
   From: https://circuitdigest.com/microcontroller-projects/arduino-spi-communication-tutorial
   Modified by Forrest Lee Erickson 20220523
   Change to Controller/Peripheral termonology
   Change variable names for start with lowercase. Constants to upper case.
   Peripherial Arduino Code:
   License: Dedicated to the Public Domain
   Warrenty: This program is designed to kill and render the earth uninhabitable,
   however it is not guaranteed to do so.
   20220524 Get working with the SPI_CONTROLER sketch. Made function updateFromSPI().
   20220925 Changes for GPAD Version 1 PCB.  SS on pin 7 and LED_PIN on D3.
   20220927 Change back for GPAD nCS on Pin 10
*/

// SPI PERIPHERAL (ARDUINO UNO)
// SPI COMMUNICATION BETWEEN TWO ARDUINO UNOs
// CIRCUIT DIGEST

/* Hardware Notes Peripheral
   SPI Line Pin in Arduino, IO setup
  MOSI 11 or ICSP-4  Input
  MISO 12 or ICSP-1 Output
  SCK 13 or ICSP-3  Input
  SS 10 Input
*/

#define GPAD_VERSION1

#define DEBUG_SPI 0

// #define DEBUG 0
#define DEBUG 1
// #define DEBUG 4

unsigned long last_command_ms;

// We have currently defined our alam time to include 10-second "songs",
// So we will not process a new alarm condition until we have completed one song.
const unsigned long DELAY_BEFORE_NEW_COMMAND_ALLOWED = 10000;
const unsigned int NUM_WIFI_RECONNECT_RETRIES = 3;

const int LED_PINS[] = {LIGHT0, LIGHT1, LIGHT2, LIGHT3, LIGHT4};
// const int SWITCH_PINS[] = { SW1, SW2, SW3, SW4 };  // SW1, SW2, SW3, SW4
const int LED_COUNT = sizeof(LED_PINS) / sizeof(LED_PINS[0]);
// const int SWITCH_COUNT = sizeof(SWITCH_PINS) / sizeof(SWITCH_PINS[0]);

// MQTT Broker
// #define USE_HIVEMQ
#ifdef USE_HIVEMQ
const char *DEFAULT_MQTT_BROKER_NAME = "broker.hivemq.com";
const char *mqtt_user = "";
const char *mqtt_password = "";
#else
const char *DEFAULT_MQTT_BROKER_NAME = "public.cloud.shiftr.io";
const char *mqtt_user = "public";
const char *mqtt_password = "public";
#endif
const size_t MQTT_BROKER_MAX_LEN = 64;
const char *MQTT_CONFIG_PATH = "/mqtt.json";
char mqtt_broker_name[MQTT_BROKER_MAX_LEN] = {0};
const uint8_t MAX_EXTRA_TOPICS = 4;
const size_t MAX_TOPIC_LEN = 64;
char subscribe_Extra_Topics[MAX_EXTRA_TOPICS][MAX_TOPIC_LEN];
uint8_t subscribe_Extra_Topic_Count = 0;
const uint8_t MAX_PUBLISH_TOPICS = 8;
char publish_Saved_Topics[MAX_PUBLISH_TOPICS][MAX_TOPIC_LEN];
uint8_t publish_Saved_Topic_Count = 0;
char publish_Default_Topic[MAX_TOPIC_LEN] = {0};
const size_t DEVICE_ROLE_MAX_LEN = 12;
char device_role[DEVICE_ROLE_MAX_LEN] = "Krake";
const char *STATUS_DISCOVERY_TOPIC = "#";
const uint8_t MAX_WATCH_TOPICS = 12;
char watchedTopics[MAX_WATCH_TOPICS][MAX_TOPIC_LEN];
uint8_t watchedTopicCount = 0;
unsigned long wifiResetRequestedAtMs = 0;

const uint8_t MAX_TRACKED_KRAKES = 16;
const unsigned long KRAKE_ONLINE_TIMEOUT_MS = 30000;
const unsigned long TOPIC_PARTICIPATION_TIMEOUT_MS = 120000;
struct TrackedKrake
{
  bool inUse;
  char id[17];
  int rssi;
  unsigned long lastSeenMs;
  unsigned long lastStatusMs;
  unsigned long watchedTopicSeenMs;
  char status[80];
  char lastTopic[MAX_TOPIC_LEN];
};
TrackedKrake trackedKrakes[MAX_TRACKED_KRAKES];

String jsonEscape(const String &raw);
bool endsWithAckTopic(const char *topic);
bool isAllowedPublishTopic(const String &topic);
bool extractJsonString(const String &json, const char *key, String &value, int startPos = 0, int *valueEndPos = nullptr);
bool writeMqttConfig();
bool loadMqttConfig();
bool parseCsvIntoTopics(const String &rawTopics, char dest[][MAX_TOPIC_LEN], uint8_t &count, uint8_t maxTopics);
String joinTopicsCsv(const char topics[][MAX_TOPIC_LEN], uint8_t count);

const size_t MAC_ADDRESS_STRING_LENGTH = 13;
// MQTT Topics, MAC plus an extension
// A MAC addresss treated as a string has 12 chars.
// The strings "_ALM" and "_ACK" have 4 chars.
// A null character is one other. 12 + 4 + 1 = 17
char subscribe_Alarm_Topic[MAX_TOPIC_LEN];
char publish_Ack_Topic[MAX_TOPIC_LEN];
char macAddressString[MAC_ADDRESS_STRING_LENGTH];

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MACSTR_PLN "%02X%02X%02X%02X%02X%02X"

// // Initialize WiFi and MQTT clients
// WiFiClient espClient;
// PubSubClient client(espClient);

// String myMAC = "";

// #define VERSION 0.02             //Version of this software
#define BAUDRATE 115200
// #define BAUDRATE 57600
// Let's have a 10 minute time out to allow people to compose strings by hand, but not
// to leave data our there forever
// #define SERIAL_TIMEOUT_MS 600000
#define SERIAL_TIMEOUT_MS 1000

// Set LED wink parameters
const int HIGH_TIME_LED_MS = 800; // time in milliseconds
const int LOW_TIME_LED_MS = 200;
unsigned long lastLEDtime_ms = 0;
// unsigned long nextLEDchangee_ms = 100; //time in ms.
unsigned long nextLEDchangee_ms = 5000; // time in ms.

//wifi disconnect event
void onWiFiDisconnect(WiFiEvent_t event, WiFiEventInfo_t info) {
  debugSerial.print("\nWifi Disconnected. Reason: ");
  debugSerial.println(info.wifi_sta_disconnected.reason);
  
  // OPTION A: Simple Reconnect
  //WiFi.begin(); 
  
  // OPTION B: Re-trigger WiFiManager Portal
  // wm.startConfigPortal("AutoConnectAP");
}


// extern int LIGHT[];
// extern int NUM_LIGHTS;

void serialSplash()
{
  // Serial splash
  debugSerial.println(F("==================================="));
  debugSerial.println(COMPANY_NAME);
  debugSerial.println(MODEL_NAME);
  //  debugSerial.println(DEVICE_UNDER_TEST);
  debugSerial.print(PROG_NAME);
  debugSerial.println(FIRMWARE_VERSION);
  //  debugSerial.println(HARDWARE_VERSION);
  debugSerial.print("Builtin ESP32 MAC Address: ");
  debugSerial.println(macAddressString);
  debugSerial.print(F("Alarm Topic: "));
  debugSerial.println(subscribe_Alarm_Topic);
  debugSerial.print(F("Broker: "));
  debugSerial.println(mqtt_broker_name);
  debugSerial.print(F("Compiled at: "));
  debugSerial.println(F(__DATE__ " " __TIME__)); // compile date that is used for a unique identifier
  debugSerial.println(LICENSE);
  debugSerial.println(F("==================================="));
  debugSerial.println();
}

// A periodic message identifying the subscriber (Krake) is on line.
void publishOnLineMsg(void)
{
  const unsigned long MESSAGE_PERIOD = 10000;
  static unsigned long lastMillis = 0; // Sets timing for periodic MQTT publish message
  // publish a message roughly every second.
  if ((millis() - lastMillis > MESSAGE_PERIOD) || (millis() < lastMillis))
  { // Check for role over.
    lastMillis = lastMillis + MESSAGE_PERIOD;

    float rssi = WiFi.RSSI();
    char rssiString[8];

#if (DEBUG > 1)
    debugSerial.print("Publish RSSI: ");
    debugSerial.println(rssi);
#endif

    dtostrf(rssi, 1, 2, rssiString);
    char onLineMsg[32] = " online, RSSI:";
    strcat(onLineMsg, rssiString);
    client.publish(publish_Ack_Topic, onLineMsg, true);

    // This should be moved to a place after the WiFi connect success
    //  debugSerial.print("Device connected at IPaddress: "); //FLE
    // debugSerial.println(WiFi.localIP());  //FLE

#if defined(HMWK)
    digitalWrite(LED_D9, !digitalRead(LED_D9)); // Toggle
#endif
  }
}

// TODO: have this return a success or failure status and move
// the delay up.
void reconnect()
{
  int n = 0;
  const String clientId = String(COMPANY_NAME) + "-" + String(macAddressString);
  const String willPayload = String(device_role) + " offline";
  while (!client.connected() && n < NUM_WIFI_RECONNECT_RETRIES)
  {
    n++;
    debugSerial.print("Attempting MQTT connection at: ");
    debugSerial.print(millis());
    debugSerial.print("..... ");
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password, publish_Ack_Topic, 1, true, willPayload.c_str()))
    {
      debugSerial.print("success at: ");
      debugSerial.println(millis());
      String onlinePayload = String(device_role) + " online";
      client.publish(publish_Ack_Topic, onlinePayload.c_str(), true);
      client.subscribe(subscribe_Alarm_Topic); // Subscribe to GPAD API alarms
      client.subscribe(STATUS_DISCOVERY_TOPIC);
      for (uint8_t i = 0; i < subscribe_Extra_Topic_Count; i++)
      {
        client.subscribe(subscribe_Extra_Topics[i]);
      }
      for (uint8_t i = 0; i < watchedTopicCount; i++)
      {
        client.subscribe(watchedTopics[i]);
      }
    }
    else
    {
      debugSerial.print("failed, rc=");
      debugSerial.println(client.state());
      delay(1000);
    }
  }
  debugSerial.println((client.connected()) ? "connected!" : "failed to reconnect!");
}

bool isManagedSubscribedTopic(const char *topic)
{
  if (strcmp(topic, STATUS_DISCOVERY_TOPIC) == 0)
  {
    return true;
  }
  if (endsWithAckTopic(topic))
  {
    return true;
  }
  if (strcmp(topic, subscribe_Alarm_Topic) == 0)
  {
    return true;
  }
  for (uint8_t i = 0; i < watchedTopicCount; i++)
  {
    if (strcmp(topic, watchedTopics[i]) == 0)
    {
      return true;
    }
  }
  for (uint8_t i = 0; i < subscribe_Extra_Topic_Count; i++)
  {
    if (strcmp(topic, subscribe_Extra_Topics[i]) == 0)
    {
      return true;
    }
  }
  return false;
}

bool endsWithAckTopic(const char *topic)
{
  const size_t len = strlen(topic);
  if (len < 4)
  {
    return false;
  }
  return strcmp(topic + len - 4, "_ACK") == 0;
}

void clearTrackedKrakes()
{
  for (uint8_t i = 0; i < MAX_TRACKED_KRAKES; i++)
  {
    trackedKrakes[i].inUse = false;
    trackedKrakes[i].id[0] = '\0';
    trackedKrakes[i].status[0] = '\0';
    trackedKrakes[i].lastTopic[0] = '\0';
    trackedKrakes[i].rssi = 0;
    trackedKrakes[i].lastSeenMs = 0;
    trackedKrakes[i].lastStatusMs = 0;
    trackedKrakes[i].watchedTopicSeenMs = 0;
  }
}

int indexForKrake(const String &krakeId)
{
  if (krakeId.length() == 0)
  {
    return -1;
  }

  int freeSlot = -1;
  for (uint8_t i = 0; i < MAX_TRACKED_KRAKES; i++)
  {
    if (trackedKrakes[i].inUse && krakeId.equalsIgnoreCase(trackedKrakes[i].id))
    {
      return i;
    }
    if (!trackedKrakes[i].inUse && freeSlot < 0)
    {
      freeSlot = i;
    }
  }

  if (freeSlot >= 0)
  {
    trackedKrakes[freeSlot].inUse = true;
    krakeId.toCharArray(trackedKrakes[freeSlot].id, sizeof(trackedKrakes[freeSlot].id));
    return freeSlot;
  }

  return -1;
}

String extractKrakeIdFromAckTopic(const char *topic)
{
  String id(topic);
  id.toUpperCase();
  if (id.endsWith("_ACK"))
  {
    id.remove(id.length() - 4);
  }
  return id;
}

int parseRssiFromStatus(const String &status)
{
  const int marker = status.indexOf("RSSI:");
  if (marker < 0)
  {
    return 0;
  }
  String rssiPart = status.substring(marker + 5);
  rssiPart.trim();
  return rssiPart.toInt();
}

void updateKrakeStatusFromAck(const char *topic, const String &statusMsg)
{
  const String krakeId = extractKrakeIdFromAckTopic(topic);
  const int idx = indexForKrake(krakeId);
  if (idx < 0)
  {
    return;
  }

  trackedKrakes[idx].lastSeenMs = millis();
  trackedKrakes[idx].lastStatusMs = millis();
  trackedKrakes[idx].rssi = parseRssiFromStatus(statusMsg);
  strncpy(trackedKrakes[idx].status, statusMsg.c_str(), sizeof(trackedKrakes[idx].status) - 1);
  trackedKrakes[idx].status[sizeof(trackedKrakes[idx].status) - 1] = '\0';
  strncpy(trackedKrakes[idx].lastTopic, topic, sizeof(trackedKrakes[idx].lastTopic) - 1);
  trackedKrakes[idx].lastTopic[sizeof(trackedKrakes[idx].lastTopic) - 1] = '\0';
}

bool isWatchedTopic(const char *topic)
{
  for (uint8_t i = 0; i < watchedTopicCount; i++)
  {
    if (strcmp(topic, watchedTopics[i]) == 0)
    {
      return true;
    }
  }
  return false;
}

String extractKrakeIdFromTopic(const char *topic)
{
  String id = String(topic);
  const int slash = id.indexOf('/');
  if (slash > 0)
  {
    id = id.substring(0, slash);
  }
  const int underscore = id.indexOf('_');
  if (underscore > 0)
  {
    id = id.substring(0, underscore);
  }
  id.trim();
  id.toUpperCase();
  return id;
}

void markWatchedTopicParticipant(const char *topic)
{
  if (!isWatchedTopic(topic))
  {
    return;
  }

  String krakeId = extractKrakeIdFromTopic(topic);
  if (krakeId.length() == 0)
  {
    return;
  }

  const int idx = indexForKrake(krakeId);
  if (idx < 0)
  {
    return;
  }
  trackedKrakes[idx].watchedTopicSeenMs = millis();
  trackedKrakes[idx].lastSeenMs = millis();
  strncpy(trackedKrakes[idx].lastTopic, topic, sizeof(trackedKrakes[idx].lastTopic) - 1);
  trackedKrakes[idx].lastTopic[sizeof(trackedKrakes[idx].lastTopic) - 1] = '\0';
}

String joinedWatchedTopics()
{
  String result;
  for (uint8_t i = 0; i < watchedTopicCount; i++)
  {
    if (i > 0)
    {
      result += ",";
    }
    result += watchedTopics[i];
  }
  return result;
}

void clearWatchedTopics()
{
  watchedTopicCount = 0;
  for (uint8_t i = 0; i < MAX_WATCH_TOPICS; i++)
  {
    watchedTopics[i][0] = '\0';
  }
}

bool setWatchedTopics(const String &rawTopics)
{
  clearWatchedTopics();
  String token = "";
  for (size_t i = 0; i <= rawTopics.length(); i++)
  {
    const char c = (i == rawTopics.length()) ? ',' : rawTopics.charAt(i);
    if (c == ',' || c == '\n' || c == '\r' || c == '\t')
    {
      token.trim();
      if (token.length() > 0)
      {
        if (token.length() >= MAX_TOPIC_LEN || watchedTopicCount >= MAX_WATCH_TOPICS)
        {
          return false;
        }
        token.toCharArray(watchedTopics[watchedTopicCount], MAX_TOPIC_LEN);
        watchedTopicCount++;
      }
      token = "";
      continue;
    }
    token += c;
  }
  return true;
}

String trackedKrakesJson()
{
  const unsigned long now = millis();
  String payload = "{\"watchedTopic\":\"";
  payload += jsonEscape(watchedTopicCount > 0 ? String(watchedTopics[0]) : String(""));
  payload += "\",\"watchedTopics\":\"";
  payload += jsonEscape(joinedWatchedTopics());
  payload += "\",\"mqttConnected\":" + String(client.connected() ? "true" : "false") + ",";
  payload += "\"broker\":\"" + jsonEscape(String(mqtt_broker_name)) + "\",";
  payload += "\"subscribeAlarmTopic\":\"" + jsonEscape(String(subscribe_Alarm_Topic)) + "\",";
  payload += "\"publishAckTopic\":\"" + jsonEscape(String(publish_Ack_Topic)) + "\",";
  payload += "\"publishDefaultTopic\":\"" + jsonEscape(String(publish_Default_Topic)) + "\",";
  payload += "\"publishTopics\":\"" + jsonEscape(joinedPublishTopics()) + "\",";
  payload += "\"subscribeTopics\":\"" + jsonEscape(joinedExtraTopics()) + "\",";
  payload += "\"role\":\"" + jsonEscape(String(device_role)) + "\",";
  payload += "\"krakes\":[";
  bool first = true;

  for (uint8_t i = 0; i < MAX_TRACKED_KRAKES; i++)
  {
    if (!trackedKrakes[i].inUse)
    {
      continue;
    }
    const bool online = (now - trackedKrakes[i].lastStatusMs) <= KRAKE_ONLINE_TIMEOUT_MS;
    const bool topicParticipant = trackedKrakes[i].watchedTopicSeenMs > 0 &&
                                  (now - trackedKrakes[i].watchedTopicSeenMs) <= TOPIC_PARTICIPATION_TIMEOUT_MS;

    if (!first)
    {
      payload += ",";
    }
    first = false;
    payload += "{";
    payload += "\"id\":\"" + jsonEscape(String(trackedKrakes[i].id)) + "\",";
    payload += "\"online\":" + String(online ? "true" : "false") + ",";
    payload += "\"topicParticipant\":" + String(topicParticipant ? "true" : "false") + ",";
    payload += "\"rssi\":" + String(trackedKrakes[i].rssi) + ",";
    payload += "\"status\":\"" + jsonEscape(String(trackedKrakes[i].status)) + "\",";
    payload += "\"lastTopic\":\"" + jsonEscape(String(trackedKrakes[i].lastTopic)) + "\",";
    payload += "\"secondsSinceStatus\":" + String((now - trackedKrakes[i].lastStatusMs) / 1000) + ",";
    payload += "\"secondsSinceTopic\":" + String((trackedKrakes[i].watchedTopicSeenMs == 0) ? -1 : ((now - trackedKrakes[i].watchedTopicSeenMs) / 1000));
    payload += "}";
  }

  payload += "]}";
  return payload;
}

void clearExtraTopics()
{
  subscribe_Extra_Topic_Count = 0;
  for (uint8_t i = 0; i < MAX_EXTRA_TOPICS; i++)
  {
    subscribe_Extra_Topics[i][0] = '\0';
  }
}

void parseAndSetExtraTopics(const String &rawTopics)
{
  clearExtraTopics();
  String token = "";
  for (size_t i = 0; i <= rawTopics.length(); i++)
  {
    char c = (i == rawTopics.length()) ? ',' : rawTopics.charAt(i);
    if (c == ',' || c == '\n' || c == '\r' || c == '\t')
    {
      token.trim();
      if (token.length() > 0 && subscribe_Extra_Topic_Count < MAX_EXTRA_TOPICS)
      {
        token.toCharArray(subscribe_Extra_Topics[subscribe_Extra_Topic_Count], MAX_TOPIC_LEN);
        subscribe_Extra_Topic_Count++;
      }
      token = "";
      continue;
    }
    token += c;
  }
}

String joinedExtraTopics()
{
  String result;
  for (uint8_t i = 0; i < subscribe_Extra_Topic_Count; i++)
  {
    if (i > 0)
    {
      result += ",";
    }
    result += subscribe_Extra_Topics[i];
  }
  return result;
}

String joinedPublishTopics()
{
  return joinTopicsCsv(publish_Saved_Topics, publish_Saved_Topic_Count);
}

void clearPublishTopics()
{
  publish_Saved_Topic_Count = 0;
  for (uint8_t i = 0; i < MAX_PUBLISH_TOPICS; i++)
  {
    publish_Saved_Topics[i][0] = '\0';
  }
}

bool parseAndSetPublishTopics(const String &rawTopics)
{
  return parseCsvIntoTopics(rawTopics, publish_Saved_Topics, publish_Saved_Topic_Count, MAX_PUBLISH_TOPICS);
}

bool isAllowedPublishTopic(const String &topic)
{
  if (topic.length() == 0 || topic.length() >= MAX_TOPIC_LEN)
  {
    return false;
  }

  for (uint8_t i = 0; i < publish_Saved_Topic_Count; i++)
  {
    if (topic.equals(publish_Saved_Topics[i]))
    {
      return true;
    }
  }

  return topic.equals(publish_Default_Topic);
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
  const int quoteStart = json.indexOf('"', colonPos + 1);
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

bool extractJsonString(const String &json, const char *key, String &value, int startPos, int *valueEndPos)
{
  const String keyToken = String("\"") + key + "\"";
  const int keyPos = json.indexOf(keyToken, startPos);
  if (keyPos < 0)
  {
    return false;
  }
  return parseJsonStringAt(json, keyPos + keyToken.length(), value, valueEndPos);
}

String joinTopicsCsv(const char topics[][MAX_TOPIC_LEN], uint8_t count)
{
  String result;
  for (uint8_t i = 0; i < count; i++)
  {
    if (i > 0)
    {
      result += ",";
    }
    result += topics[i];
  }
  return result;
}

bool parseCsvIntoTopics(const String &rawTopics, char dest[][MAX_TOPIC_LEN], uint8_t &count, uint8_t maxTopics)
{
  count = 0;
  for (uint8_t i = 0; i < maxTopics; i++)
  {
    dest[i][0] = '\0';
  }

  String token;
  for (size_t i = 0; i <= rawTopics.length(); i++)
  {
    const char c = (i == rawTopics.length()) ? ',' : rawTopics.charAt(i);
    if (c == ',' || c == '\n' || c == '\r' || c == '\t')
    {
      token.trim();
      if (token.length() > 0)
      {
        if (token.length() >= MAX_TOPIC_LEN || count >= maxTopics)
        {
          return false;
        }
        token.toCharArray(dest[count], MAX_TOPIC_LEN);
        count++;
      }
      token = "";
      continue;
    }
    token += c;
  }
  return true;
}

bool writeMqttConfig()
{
  File file = LittleFS.open(MQTT_CONFIG_PATH, "w");
  if (!file)
  {
    debugSerial.println(F("Failed to open /mqtt.json for write"));
    return false;
  }

  String payload = "{";
  payload += "\"broker\":\"" + jsonEscape(String(mqtt_broker_name)) + "\",";
  payload += "\"subscribeTopic\":\"" + jsonEscape(String(subscribe_Alarm_Topic)) + "\",";
  payload += "\"publishTopic\":\"" + jsonEscape(String(publish_Ack_Topic)) + "\",";
  payload += "\"subscribeTopics\":\"" + jsonEscape(joinedExtraTopics()) + "\",";
  payload += "\"watchTopics\":\"" + jsonEscape(joinedWatchedTopics()) + "\",";
  payload += "\"publishTopics\":\"" + jsonEscape(joinedPublishTopics()) + "\",";
  payload += "\"publishDefaultTopic\":\"" + jsonEscape(String(publish_Default_Topic)) + "\",";
  payload += "\"role\":\"" + jsonEscape(String(device_role)) + "\"";
  payload += "}";

  const size_t written = file.print(payload);
  file.close();
  return written == payload.length();
}

bool loadMqttConfig()
{
  if (!LittleFS.exists(MQTT_CONFIG_PATH))
  {
    return false;
  }

  File file = LittleFS.open(MQTT_CONFIG_PATH, "r");
  if (!file)
  {
    debugSerial.println(F("Failed to open /mqtt.json for read"));
    return false;
  }

  const String content = file.readString();
  file.close();

  String value;
  if (extractJsonString(content, "broker", value))
  {
    value.trim();
    if (value.length() > 0 && value.length() < MQTT_BROKER_MAX_LEN)
    {
      value.toCharArray(mqtt_broker_name, MQTT_BROKER_MAX_LEN);
    }
  }

  if (extractJsonString(content, "subscribeTopic", value))
  {
    value.trim();
    if (value.length() > 0 && value.length() < sizeof(subscribe_Alarm_Topic))
    {
      value.toCharArray(subscribe_Alarm_Topic, sizeof(subscribe_Alarm_Topic));
    }
  }

  if (extractJsonString(content, "publishTopic", value))
  {
    value.trim();
    if (value.length() > 0 && value.length() < sizeof(publish_Ack_Topic))
    {
      value.toCharArray(publish_Ack_Topic, sizeof(publish_Ack_Topic));
    }
  }

  if (extractJsonString(content, "subscribeTopics", value))
  {
    parseAndSetExtraTopics(value);
  }
  if (extractJsonString(content, "watchTopics", value))
  {
    setWatchedTopics(value);
  }
  if (extractJsonString(content, "publishTopics", value))
  {
    parseAndSetPublishTopics(value);
  }
  if (extractJsonString(content, "publishDefaultTopic", value))
  {
    value.trim();
    if (value.length() > 0 && value.length() < MAX_TOPIC_LEN)
    {
      value.toCharArray(publish_Default_Topic, MAX_TOPIC_LEN);
    }
  }
  if (extractJsonString(content, "role", value))
  {
    value.trim();
    if (value.length() > 0 && value.length() < DEVICE_ROLE_MAX_LEN)
    {
      value.toCharArray(device_role, DEVICE_ROLE_MAX_LEN);
    }
  }
  return true;
}

bool parseMutedParam(const String &rawValue, bool &parsedMutedState)
{
  String normalized = rawValue;
  normalized.trim();
  normalized.toLowerCase();

  if (normalized == "1" || normalized == "true" || normalized == "on" || normalized == "mute" || normalized == "muted")
  {
    parsedMutedState = true;
    return true;
  }

  if (normalized == "0" || normalized == "false" || normalized == "off" || normalized == "unmute" || normalized == "unmuted")
  {
    parsedMutedState = false;
    return true;
  }

  return false;
}

bool applyMuteSetting(const String &rawValue)
{
  bool requestedMutedState = false;
  if (!parseMutedParam(rawValue, requestedMutedState))
  {
    return false;
  }
  setMuted(requestedMutedState);
  return true;
}

bool applyBrokerSetting(const String &broker)
{
  if (broker.length() == 0 || broker.length() >= MQTT_BROKER_MAX_LEN)
  {
    return false;
  }

  broker.toCharArray(mqtt_broker_name, MQTT_BROKER_MAX_LEN);
  client.setServer(mqtt_broker_name, 1883);
  if (client.connected())
  {
    client.disconnect();
  }
  reconnect();
  writeMqttConfig();
  return true;
}

bool applyRoleSetting(const String &rawRole)
{
  String role = rawRole;
  role.trim();
  if (role.equalsIgnoreCase("Krake"))
  {
    strncpy(device_role, "Krake", DEVICE_ROLE_MAX_LEN - 1);
    device_role[DEVICE_ROLE_MAX_LEN - 1] = '\0';
    return true;
  }
  return false;
}

bool applyPrimaryTopicSetting(const String &rawTopic, char *destTopic, size_t destLen)
{
  String topic = rawTopic;
  topic.trim();
  if (topic.length() == 0 || topic.length() >= destLen)
  {
    return false;
  }
  topic.toCharArray(destTopic, destLen);
  return true;
}

void applyExtraTopicsSetting(const String &topics)
{
  parseAndSetExtraTopics(topics);
  if (client.connected())
  {
    client.disconnect();
  }
  reconnect();
  writeMqttConfig();
}
// Function to turn on all lamps
void turnOnAllLamps()
{
#if defined(HMWK)
  digitalWrite(LED_D9, HIGH);
#endif
  digitalWrite(LIGHT0, HIGH);
  digitalWrite(LIGHT1, HIGH);
  digitalWrite(LIGHT2, HIGH);
  digitalWrite(LIGHT3, HIGH);
  digitalWrite(LIGHT4, HIGH);
}
void turnOffAllLamps()
{
#if defined(HMWK)
  digitalWrite(LED_D9, LOW);
#endif
  digitalWrite(LIGHT0, LOW);
  digitalWrite(LIGHT1, LOW);
  digitalWrite(LIGHT2, LOW);
  digitalWrite(LIGHT3, LOW);
  digitalWrite(LIGHT4, LOW);
}

// Handeler for MQTT subscribed messages
void callback(char *topic, byte *payload, unsigned int length)
{
  // todo, remove use of String here....
  // Note: We will check for topic or topics in the future...
  if (isManagedSubscribedTopic(topic))
  {
    const bool ackTopic = endsWithAckTopic(topic);
    char mbuff[121];

    // Put payload into mbuff[] a character array
    int m = min((unsigned int)length, (unsigned int)120);
    for (int i = 0; i < m; i++)
    {
      mbuff[i] = (char)payload[i];
    }
    mbuff[m] = '\0';

    if (!ackTopic)
    {
      debugSerial.print("Topic arrived [");
      debugSerial.print(topic);
      debugSerial.print("] ");
#if (DEBUG > 0)
      debugSerial.print("|");
      debugSerial.print(mbuff);
      debugSerial.println("|");
#endif
      debugSerial.println("Received MQTT Msg.");
    }

    const String payloadText = String(mbuff);
    if (ackTopic)
    {
      updateKrakeStatusFromAck(topic, payloadText);
      return;
    }
    markWatchedTopicParticipant(topic);
    interpretBuffer(mbuff, m, &debugSerial, &client); // Process the MQTT message
    annunciateAlarmLevel(&debugSerial);
  }
} // end call back

bool readMacAddress(uint8_t *baseMac)
{
  //  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK)
  {
    // debugSerial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
    //               baseMac[0], baseMac[1], baseMac[2],
    //               baseMac[3], baseMac[4], baseMac[5]);
    return true;
  }
  else
  {
    // debugSerial.println("Failed to read MAC address");
    return false;
  }
}

String jsonEscape(const String &raw)
{
  String escaped;
  escaped.reserve(raw.length() + 8);
  for (size_t i = 0; i < raw.length(); i++)
  {
    const char c = raw.charAt(i);
    if (c == '\\' || c == '"')
    {
      escaped += '\\';
    }
    escaped += c;
  }
  return escaped;
}

String uptimeString()
{
  const uint32_t totalSeconds = millis() / 1000;
  const uint32_t hours = totalSeconds / 3600;
  const uint32_t minutes = (totalSeconds % 3600) / 60;
  const uint32_t seconds = totalSeconds % 60;
  return String(hours) + "h " + String(minutes) + "m " + String(seconds) + "s";
}

String currentUrl()
{
  IPAddress ip = wifiManager.getAddress();
  if (ip == INADDR_NONE)
  {
    return String("-");
  }
  return "http://" + ip.toString();
}

String templateProcessor(const String &var)
{
  if (var == "SERIAL")
  {
    return String(macAddressString);
  }
  if (var == "URL")
  {
    return currentUrl();
  }
  if (var == "IP")
  {
    return wifiManager.getAddress().toString();
  }
  if (var == "MAC")
  {
    return String(macAddressString);
  }
  if (var == "RSSI")
  {
    return String(WiFi.RSSI()) + " dBm";
  }
  if (var == "UPTIME")
  {
    return uptimeString();
  }
  if (var == "MQTT")
  {
    return client.connected() ? String("connected") : String("disconnected");
  }
  if (var == "FIRMWARE_VERSION")
  {
    return String(FIRMWARE_VERSION);
  }
  if (var == "COMPILED_AT")
  {
    return String(__DATE__ " " __TIME__);
  }
  if (var == "SERIAL_PORT")
  {
    return String("UART0 (USB Serial/JTAG)");
  }
  if (var == "SERIAL_BAUD")
  {
    return String(BAUDRATE);
  }
  if (var == "QR")
  {
    return "/favicon.png";
  }
  return WifiOTA::processor(var);
}

// Elegant OTA Setup

void setupOTA()
{

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html", false, templateProcessor); });

  server.on("/monitor", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/monitor.html", "text/html", false, templateProcessor); });

  server.on("/device-monitor", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/device-monitor.html", "text/html"); });

  server.on("/device-monitor.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/device-monitor.html", "text/html"); });

  server.on("/lcd", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String payload = "{\"lines\":[\"";
              payload += jsonEscape(lcd.line(0));
              payload += "\",\"";
              payload += jsonEscape(lcd.line(1));
              payload += "\",\"";
              payload += jsonEscape(lcd.line(2));
              payload += "\",\"";
              payload += jsonEscape(lcd.line(3));
              payload += "\"]}";
              request->send(200, "application/json", payload); });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String payload = "{";
              payload += "\"ip\":\"" + jsonEscape(wifiManager.getAddress().toString()) + "\",";
              payload += "\"serial\":\"" + jsonEscape(String(macAddressString)) + "\",";
              payload += "\"mac\":\"" + jsonEscape(String(macAddressString)) + "\",";
              payload += "\"rssi\":\"" + String(WiFi.RSSI()) + " dBm\",";
              payload += "\"uptime\":\"" + jsonEscape(uptimeString()) + "\",";
              payload += "\"mqtt\":\"" + String(client.connected() ? "connected" : "disconnected") + "\",";
              payload += "\"firmware\":\"" + jsonEscape(String(FIRMWARE_VERSION)) + "\",";
              payload += "\"compiled\":\"" + jsonEscape(String(__DATE__ " " __TIME__)) + "\",";
              payload += "\"serialPort\":\"" + jsonEscape(String("UART0 (USB Serial/JTAG)")) + "\",";
              payload += "\"serialBaud\":\"" + String(BAUDRATE) + "\",";
              payload += "\"url\":\"" + jsonEscape(currentUrl()) + "\"";
              payload += "}";
              request->send(200, "application/json", payload); });

  server.on("/serial-monitor", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send(200, "text/plain", serialLogBuffer); });

  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/settings.html", "text/html"); });

  server.on("/setup", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/setup.html", "text/html"); });

  server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String ssid;
              String password;
              const bool hasStored = wifiManager.loadCredentials(ssid, password);
              std::vector<WifiOTA::Manager::Credential> credentials;
              wifiManager.loadCredentialsList(credentials);
              String payload = "{";
              payload += "\"hasStored\":" + String(hasStored ? "true" : "false") + ",";
              payload += "\"ssid\":\"" + jsonEscape(ssid) + "\",";
              payload += "\"count\":" + String(credentials.size());
              payload += "}";
              request->send(200, "application/json", payload); });

  server.on("/wifi", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              if (!request->hasParam("ssid", true))
              {
                request->send(400, "text/plain", "missing ssid");
                return;
              }
              const String ssid = request->getParam("ssid", true)->value();
              if (!request->hasParam("password", true))
              {
                request->send(400, "text/plain", "missing password");
                return;
              }
              const String password = request->getParam("password", true)->value();
              String trimmedSsid = ssid;
              String trimmedPassword = password;
              trimmedSsid.trim();
              trimmedPassword.trim();
              if (trimmedSsid.length() == 0)
              {
                request->send(400, "text/plain", "ssid cannot be empty");
                return;
              }
              if (trimmedPassword.length() == 0)
              {
                request->send(400, "text/plain", "password cannot be empty");
                return;
              }
              if (!wifiManager.saveCredentials(trimmedSsid, trimmedPassword))
              {
                request->send(500, "text/plain", "failed to save wifi.json");
                return;
              }
              request->send(200, "text/plain", "wifi.json saved"); });

  server.on("/manual", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/manual.html", "text/html"); });

  server.on("/PMD_GPAD_API", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/PMD_GPAD_API.html", "text/html"); });

  server.on("/PMD_GPAD_API.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/PMD_GPAD_API.html", "text/html"); });

  server.on("/settings-data", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String payload = "{";
              payload += "\"broker\":\"" + jsonEscape(String(mqtt_broker_name)) + "\",";
              payload += "\"subscribeTopic\":\"" + jsonEscape(String(subscribe_Alarm_Topic)) + "\",";
              payload += "\"publishTopic\":\"" + jsonEscape(String(publish_Ack_Topic)) + "\",";
              payload += "\"extraTopics\":\"" + jsonEscape(joinedExtraTopics()) + "\",";
              payload += "\"watchTopics\":\"" + jsonEscape(joinedWatchedTopics()) + "\",";
              payload += "\"publishTopics\":\"" + jsonEscape(joinedPublishTopics()) + "\",";
              payload += "\"publishDefaultTopic\":\"" + jsonEscape(String(publish_Default_Topic)) + "\",";
              payload += "\"role\":\"" + jsonEscape(String(device_role)) + "\",";
              payload += "\"muted\":" + String(isMuted() ? "true" : "false");
              payload += "}";
              request->send(200, "application/json", payload); });

  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              String errorMessage;
              if (request->hasParam("broker", true))
              {
                String broker = request->getParam("broker", true)->value();
                broker.trim();
                if (broker.length() == 0 || broker.length() >= MQTT_BROKER_MAX_LEN)
                {
                  errorMessage += "invalid broker;";
                }
                else
                {
                  broker.toCharArray(mqtt_broker_name, MQTT_BROKER_MAX_LEN);
                  client.setServer(mqtt_broker_name, 1883);
                }
              }

              if (request->hasParam("role", true))
              {
                if (!applyRoleSetting(request->getParam("role", true)->value()))
                {
                  errorMessage += "invalid role;";
                }
              }

              if (request->hasParam("subscribeTopic", true))
              {
                if (!applyPrimaryTopicSetting(request->getParam("subscribeTopic", true)->value(), subscribe_Alarm_Topic, sizeof(subscribe_Alarm_Topic)))
                {
                  errorMessage += "invalid subscribeTopic;";
                }
              }

              if (request->hasParam("publishTopic", true))
              {
                if (!applyPrimaryTopicSetting(request->getParam("publishTopic", true)->value(), publish_Ack_Topic, sizeof(publish_Ack_Topic)))
                {
                  errorMessage += "invalid publishTopic;";
                }
              }

              if (request->hasParam("subscribeTopics", true))
              {
                String topics = request->getParam("subscribeTopics", true)->value();
                parseAndSetExtraTopics(topics);
                {
                  errorMessage += "invalid subscribeTopics;";
                }
              }
              if (request->hasParam("watchTopics", true))
              {
                String topics = request->getParam("watchTopics", true)->value();
                if (!setWatchedTopics(topics))
                {
                  errorMessage += "invalid watchTopics;";
                }
              }
              if (request->hasParam("publishTopics", true))
              {
                String topics = request->getParam("publishTopics", true)->value();
                if (!parseAndSetPublishTopics(topics))
                {
                  errorMessage += "invalid publishTopics;";
                }
              }
              if (request->hasParam("publishDefaultTopic", true))
              {
                String topic = request->getParam("publishDefaultTopic", true)->value();
                topic.trim();
                if (topic.length() > 0 && topic.length() < MAX_TOPIC_LEN)
                {
                  topic.toCharArray(publish_Default_Topic, MAX_TOPIC_LEN);
                }
                else
                {
                  errorMessage += "invalid publishDefaultTopic;";
                }
              }

              if (errorMessage.length() > 0)
              {
                request->send(400, "text/plain", errorMessage);
                return;
              }

              writeMqttConfig();
              if (client.connected())
              {
                client.disconnect();
              }
              reconnect();
              request->send(200, "text/plain", "config updated"); });

  server.on("/settings/mute", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              if (!request->hasParam("muted", true))
              {
                request->send(400, "text/plain", "missing muted");
                return;
              }
              const String muted = request->getParam("muted", true)->value();
              if (!applyMuteSetting(muted))
              {
                request->send(400, "text/plain", "invalid muted value");
                return;
              }
              request->send(200, "text/plain", isMuted() ? "muted" : "unmuted"); });

  server.on("/settings/broker", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              if (!request->hasParam("broker", true))
              {
                request->send(400, "text/plain", "missing broker");
                return;
              }
              const String broker = request->getParam("broker", true)->value();
              if (!applyBrokerSetting(broker))
              {
                request->send(400, "text/plain", "invalid broker");
                return;
              }
              request->send(200, "text/plain", "broker updated"); });

  server.on("/settings/topics", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              if (!request->hasParam("topics", true))
              {
                request->send(400, "text/plain", "missing topics");
                return;
              }
              applyExtraTopicsSetting(request->getParam("topics", true)->value());
              request->send(200, "text/plain", "topics updated"); });

  server.on("/settings/wifi/reset", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              wifiResetRequestedAtMs = millis();
              request->send(200, "text/plain", "wifi reset scheduled"); });

  server.on("/broker-console", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->redirect("/settings"); });

  server.on("/broker-console/data", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", trackedKrakesJson()); });

  server.on("/broker-console/topic", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              if (!request->hasParam("topics", true) && !request->hasParam("topic", true))
              {
                request->send(400, "text/plain", "missing topics");
                return;
              }
              char previousWatchedTopics[MAX_WATCH_TOPICS][MAX_TOPIC_LEN];
              const uint8_t previousWatchedTopicCount = watchedTopicCount;
              for (uint8_t i = 0; i < MAX_WATCH_TOPICS; i++)
              {
                strncpy(previousWatchedTopics[i], watchedTopics[i], MAX_TOPIC_LEN - 1);
                previousWatchedTopics[i][MAX_TOPIC_LEN - 1] = '\0';
              }
              const String newTopics = request->hasParam("topics", true) ? request->getParam("topics", true)->value() : request->getParam("topic", true)->value();
              if (!setWatchedTopics(newTopics))
              {
                request->send(400, "text/plain", "invalid topics");
                return;
              }
              if (client.connected())
              {
                String subscribeFailures;
                for (uint8_t i = 0; i < previousWatchedTopicCount; i++)
                {
                  if (previousWatchedTopics[i][0] != '\0')
                  {
                    client.unsubscribe(previousWatchedTopics[i]);
                  }
                }
                for (uint8_t i = 0; i < watchedTopicCount; i++)
                {
                  if (!client.subscribe(watchedTopics[i]))
                  {
                    if (subscribeFailures.length() > 0)
                    {
                      subscribeFailures += ",";
                    }
                    subscribeFailures += watchedTopics[i];
                  }
                }
                if (subscribeFailures.length() > 0)
                {
                  request->send(500, "text/plain", "failed subscribe: " + subscribeFailures);
                  return;
                }
              }
              writeMqttConfig();
              request->send(200, "text/plain", "watch topics updated"); });

  server.on("/broker-console/publish", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              if (!request->hasParam("topic", true))
              {
                request->send(400, "text/plain", "missing topic");
                return;
              }
              if (!client.connected())
              {
                request->send(503, "text/plain", "mqtt disconnected");
                return;
              }
              String topic = request->getParam("topic", true)->value();
              String payload = request->hasParam("payload", true) ? request->getParam("payload", true)->value() : "";
              topic.trim();
              if (topic.length() == 0)
              {
                request->send(400, "text/plain", "topic is required");
                return;
              }
              if (!isAllowedPublishTopic(topic))
              {
                request->send(403, "text/plain", "topic not allowed; use saved broker-console publish topics");
                return;
              }
              bool ok = client.publish(topic.c_str(), payload.c_str());
              if (ok && topic.length() < MAX_TOPIC_LEN)
              {
                topic.toCharArray(publish_Default_Topic, MAX_TOPIC_LEN);
                bool exists = false;
                for (uint8_t i = 0; i < publish_Saved_Topic_Count; i++)
                {
                  if (strcmp(publish_Saved_Topics[i], publish_Default_Topic) == 0)
                  {
                    exists = true;
                    break;
                  }
                }
                if (!exists && publish_Saved_Topic_Count < MAX_PUBLISH_TOPICS)
                {
                  strncpy(publish_Saved_Topics[publish_Saved_Topic_Count], publish_Default_Topic, MAX_TOPIC_LEN - 1);
                  publish_Saved_Topics[publish_Saved_Topic_Count][MAX_TOPIC_LEN - 1] = '\0';
                  publish_Saved_Topic_Count++;
                }
                writeMqttConfig();
              }
              request->send(ok ? 200 : 500, "text/plain", ok ? "published" : "publish failed"); });

  server.serveStatic("/", LittleFS, "/");

  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(LittleFS, "/404.html", "text/html"); });

  // End of ELegant OTA Setup
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // set the LED pin mode
  digitalWrite(LED_BUILTIN, HIGH);
  // Serial setup
  delay(100);
  debugSerial.begin(BAUDRATE);
  while (!debugSerial)
  {
    ; // wait for serial port to connect. Needed for native USB
  }
  serialSplash();
  // We call this a second time to get the MAC on the screen
  // clearLCD();

// Set LED pins as outputs
#if defined(LED_D9)
  pinMode(LED_D9, OUTPUT);
#endif
  pinMode(LIGHT0, OUTPUT);
  pinMode(LIGHT1, OUTPUT);
  pinMode(LIGHT2, OUTPUT);
  pinMode(LIGHT3, OUTPUT);
  pinMode(LIGHT4, OUTPUT);
  // Turn off all LEDs initially
  turnOnAllLamps();

  // Init arrays.
  subscribe_Alarm_Topic[0] = '\0';
  publish_Ack_Topic[0] = '\0';
  macAddressString[0] = '\0';

#if (DEBUG > 1)
  debugSerial.println("Call: GPAD_HAL_setup(&debugSerial)");
#endif

  // Setup and present LCD splash screen
  // Setup the SWITCH_MUTE
  // Setup the SWITCH_ENCODER

  WifiOTA::initLittleFS();

  WiFi.onEvent(onWiFiDisconnect, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  wifiManager.initialize();

  IPAddress deviceAddress = wifiManager.getAddress();
  GPAD_HAL_setup(&debugSerial, wifiManager.getMode(), deviceAddress);

#if (DEBUG > 0)
  debugSerial.println("MAC: ");
  debugSerial.println(macAddressString);
#endif

  debugSerial.setTimeout(SERIAL_TIMEOUT_MS);
  strncpy(mqtt_broker_name, DEFAULT_MQTT_BROKER_NAME, MQTT_BROKER_MAX_LEN - 1);
  mqtt_broker_name[MQTT_BROKER_MAX_LEN - 1] = '\0';
  clearExtraTopics();
  clearPublishTopics();
  clearTrackedKrakes();
  clearWatchedTopics();
  client.setServer(mqtt_broker_name, 1883); // Default MQTT port, this is a TCP port.
  client.setCallback(callback);

#if (DEBUG > 0)
  debugSerial.println("Starting WiFi as STA");
#endif

  // Note: On Krake SN#3 only, performing this
  // while the Splash is on causes a reset, presumably
  // because too much power is drawn. I am using a conditional
  // to isolate this as much as possible, while
  // still allowing us to use a single code base for all hardware
  // devices -- rlr

#if (LIMIT_POWER_DRAW)
  clearLCD();
#endif

#if (LIMIT_POWER_DRAW)
  splashLCD();
#endif

  // setup_spi();

  uint8_t mac[6];
  readMacAddress(mac);
  char buff[13];
  sprintf(buff, MACSTR_PLN, MAC2STR(mac));

#if (DEBUG > 0)
  printf("My mac is " MACSTR "\n", MAC2STR(mac));
  debugSerial.print("MAC as char array: ");
  debugSerial.println(buff);
#endif

  strcpy(macAddressString, buff);
  macAddressString[12] = '\0';
  snprintf(subscribe_Alarm_Topic, sizeof(subscribe_Alarm_Topic), "%s_ALM", buff);
  snprintf(publish_Ack_Topic, sizeof(publish_Ack_Topic), "%s_ACK", buff);
  strncpy(publish_Default_Topic, subscribe_Alarm_Topic, MAX_TOPIC_LEN - 1);
  publish_Default_Topic[MAX_TOPIC_LEN - 1] = '\0';

  loadMqttConfig();
  client.setServer(mqtt_broker_name, 1883);

#if (DEBUG > 1)
  debugSerial.println("XXXXXXX");
  debugSerial.println(subscribe_Alarm_Topic);
  debugSerial.println(publish_Ack_Topic);
  debugSerial.println("XXXXXXX");
#endif
  // Setup SSID length is the length of the prefix, 'Krake_', which is 7
  // plus the length of the MAC address string, MAC_ADDRESS_STRING_LENGTH
  const size_t SETUP_SSID_LENGTH = 7 + MAC_ADDRESS_STRING_LENGTH;
  char setupSsid[SETUP_SSID_LENGTH] = "Krake_";
  strcat(setupSsid, macAddressString);

  // We call this a second time to get the MAC on the screen
  //  clearLCD();
  // req for Wifi Man and OTA

#if defined HMWK || defined KRAKE
  auto connectedCallback = [&]()
  {
    if (!client.connected())
    {
      reconnect();
    }

    clearLCD();
    IPAddress currentAddress = wifiManager.getAddress();
    splashLCD(wifiManager.getMode(), currentAddress);
  };
  wifiManager.setConnectedCallback(connectedCallback);
#endif

  auto apStartedCallback = [&]()
  {
    clearLCD();
    IPAddress currentAddress = wifiManager.getAddress();
    splashLCD(wifiManager.getMode(), currentAddress);
  };
  wifiManager.setApStartedCallback(apStartedCallback);

  wifiManager.connect(setupSsid);

  debugSerial.println(F("WiFi Manager connected."));

  debugSerial.println(F("initLiffleFS"));

  server.begin(); // Start server web socket to render pages
  
  debugSerial.println(F("iStart server web socket to render pages"));

  ElegantOTA.begin(&server);
  debugSerial.println(F("ElegantOTA.begin"));

  setupOTA();
  debugSerial.println(F("setupOTA"));


  initRotator();
  debugSerial.println(F("initRotator"));
  splashLCD(wifiManager.getMode(), deviceAddress);

  debugSerial.println(F("splashLCD"));

  setupDFPlayer();
  debugSerial.println(F("setupDFPlayer"));

  setup_GPAD_menu();

  debugSerial.println(F("setupGPAD_menu"));

    // Need this to work here:   printInstructions(serialport);
  debugSerial.println(F("Done With Setup!"));
  turnOnAllLamps();
  digitalWrite(LED_BUILTIN, LOW); // turn the LED off at end of setup
} // end of setup()

unsigned long last_ms = 0;
void toggle(int pin)
{
  digitalWrite(pin, digitalRead(pin) ? LOW : HIGH);
}

const unsigned long LOW_FREQ_DEBUG_MS = 20000;
unsigned long time_since_LOW_FREQ_ms = 0;

int cnt_actions = 0;

bool running_menu = false;
bool menu_just_exited = false;

void loop()
{
#if defined HMWK || defined KRAKE

  if (!client.loop())
  {
    debugSerial.print(mqtt_broker_name);
    debugSerial.print(" lost MQTT at: ");
    debugSerial.println(millis());
    reconnect();
  }

  if (wifiResetRequestedAtMs != 0 && (millis() - wifiResetRequestedAtMs) > 750)
  {
    debugSerial.println(F("Resetting WiFi credentials and restarting."));
    if (LittleFS.exists("/wifi.json"))
    {
      LittleFS.remove("/wifi.json");
      debugSerial.println(F("Removed /wifi.json"));
    }
    WiFi.disconnect(true, true);
    delay(150);
    ESP.restart();
  }

  publishOnLineMsg();
  wink(); // The builtin LED
#endif

  unchanged_anunicateAlarmLevel(&debugSerial);
  // delay(20);
  GPAD_HAL_loop();

  processSerial(&debugSerial, &debugSerial, &client);

  // Here we also process the UART1 using the same routine.
  processSerial(&debugSerial, &uartSerial1, &client);

  // Here we will listen for an SPI command...

  // // Now try to read from the SPI Port!
#if defined(GPAD)
  updateFromSPI();
#endif

  updateRotator();

  if (menu_just_exited)
  {
    lcd.clear();
    lcd.noBacklight();
    restoreAlarmLevel(&debugSerial);
    menu_just_exited = false;
  }
  if (running_menu)
  {
    lcd.backlight();
    poll_GPAD_menu();
  }

  // if ((millis() / 10000) > cnt_actions) {
  //   cnt_actions++;
  //   navigate_to_n_and_execute(cnt_actions % 3);
  // }
}
