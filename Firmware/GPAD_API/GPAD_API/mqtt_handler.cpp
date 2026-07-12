#include "mqtt_handler.h"
#include "debug_macros.h"
#include "spi_broker_mirror.h"
#include <Arduino.h>
#include <string.h>

extern char publish_Ack_Topic[];

namespace
{
  const uint8_t MQTT_QUEUE_SIZE = 8;
  PendingMqtt mqttQueue[MQTT_QUEUE_SIZE];

  void copyBounded(char *dest, size_t destLen, const char *src)
  {
    if (destLen == 0)
    {
      return;
    }
    if (src == nullptr)
    {
      dest[0] = '\0';
      return;
    }
    strncpy(dest, src, destLen - 1);
    dest[destLen - 1] = '\0';
  }
}

bool queueMqtt(const char *topic, const char *payload, bool retain)
{
  if (topic == nullptr || payload == nullptr || topic[0] == '\0')
  {
    return false;
  }

  // SPI is an independent local transport: mirror every broker-bound message
  // even while WiFi/MQTT are offline or the MQTT queue is full.
  queueSpiBrokerMirror(topic, payload, retain);

  for (uint8_t i = 0; i < MQTT_QUEUE_SIZE; i++)
  {
    if (!mqttQueue[i].active)
    {
      copyBounded(mqttQueue[i].topic, sizeof(mqttQueue[i].topic), topic);
      copyBounded(mqttQueue[i].payload, sizeof(mqttQueue[i].payload), payload);
      mqttQueue[i].retain = retain;
      mqttQueue[i].active = true;
      return true;
    }
  }

  DBG_PRINTLN(F("MQTT publish queue full"));
  return false;
}

void serviceMqttQueue(PubSubClient *client)
{
  if (client == nullptr || !client->connected())
  {
    return;
  }

  for (uint8_t i = 0; i < MQTT_QUEUE_SIZE; i++)
  {
    if (!mqttQueue[i].active)
    {
      continue;
    }

    if (client->publish(mqttQueue[i].topic, mqttQueue[i].payload, mqttQueue[i].retain))
    {
      mqttQueue[i].active = false;
    }
    return;
  }
}

bool publishAck(PubSubClient *client, const char *message)
{
  (void)client;
  if (message == nullptr)
  {
    return false;
  }

  return queueMqtt(publish_Ack_Topic, message);
}


bool publishGPAPResponse(PubSubClient *client, const char *responseType, const char *alarmId)
{
  if (client == nullptr || responseType == nullptr)
  {
    return false;
  }

  char payload[32];

  if (alarmId != nullptr && strlen(alarmId) > 0)
  {
    snprintf(payload, sizeof(payload), "o%s{%s}", responseType, alarmId);
  }
  else
  {
    snprintf(payload, sizeof(payload), "o%s", responseType);
  }

  DBG_PRINT(F("Queueing GPAP response: "));
  DBG_PRINTLN(payload);

  return queueMqtt(publish_Ack_Topic, payload);
}
