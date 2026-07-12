#include "spi_broker_mirror.h"
#include "debug_macros.h"
#include <SPI.h>
#include <string.h>

namespace
{
  const uint8_t SPI_MIRROR_QUEUE_SIZE = 8;
  const uint8_t SPI_MIRROR_SS_PIN = 15;
  const uint32_t SPI_MIRROR_CLOCK_HZ = 1000000;
  const size_t SPI_MIRROR_TOPIC_CAPACITY = 96;
  const size_t SPI_MIRROR_PAYLOAD_CAPACITY = 128;

  struct PendingSpiMirror
  {
    char topic[SPI_MIRROR_TOPIC_CAPACITY];
    char payload[SPI_MIRROR_PAYLOAD_CAPACITY];
    bool retain;
    bool active;
  };

  SPIClass spiMirror(VSPI);
  PendingSpiMirror spiMirrorQueue[SPI_MIRROR_QUEUE_SIZE];
  bool spiMirrorInitialized = false;

  void copyBounded(char *dest, size_t destLen, const char *src)
  {
    strncpy(dest, src, destLen - 1);
    dest[destLen - 1] = '\0';
  }

  void transferByte(uint8_t value)
  {
    spiMirror.transfer(value);
  }
}

void initializeSpiBrokerMirror()
{
  if (spiMirrorInitialized)
  {
    return;
  }
  pinMode(SPI_MIRROR_SS_PIN, OUTPUT);
  digitalWrite(SPI_MIRROR_SS_PIN, HIGH);
  spiMirror.begin(SCK, MISO, MOSI, SPI_MIRROR_SS_PIN);
  spiMirrorInitialized = true;
}

bool queueSpiBrokerMirror(const char *topic, const char *payload, bool retain)
{
  if (topic == nullptr || payload == nullptr || topic[0] == '\0')
  {
    return false;
  }

  for (uint8_t i = 0; i < SPI_MIRROR_QUEUE_SIZE; ++i)
  {
    if (!spiMirrorQueue[i].active)
    {
      copyBounded(spiMirrorQueue[i].topic, sizeof(spiMirrorQueue[i].topic), topic);
      copyBounded(spiMirrorQueue[i].payload, sizeof(spiMirrorQueue[i].payload), payload);
      spiMirrorQueue[i].retain = retain;
      spiMirrorQueue[i].active = true;
      return true;
    }
  }

  DBG_PRINTLN(F("SPI broker mirror queue full"));
  return false;
}

void serviceSpiBrokerMirror()
{
  if (!spiMirrorInitialized)
  {
    return;
  }

  for (uint8_t i = 0; i < SPI_MIRROR_QUEUE_SIZE; ++i)
  {
    PendingSpiMirror &pending = spiMirrorQueue[i];
    if (!pending.active)
    {
      continue;
    }

    const uint8_t topicLength = static_cast<uint8_t>(strlen(pending.topic));
    const uint8_t payloadLength = static_cast<uint8_t>(strlen(pending.payload));
    const uint8_t header[] = {'K', 'R', 'K', '1', pending.retain ? uint8_t(1) : uint8_t(0), topicLength, payloadLength};

    spiMirror.beginTransaction(SPISettings(SPI_MIRROR_CLOCK_HZ, MSBFIRST, SPI_MODE0));
    digitalWrite(SPI_MIRROR_SS_PIN, LOW);
    for (size_t j = 0; j < sizeof(header); ++j)
    {
      transferByte(header[j]);
    }
    for (uint8_t j = 0; j < topicLength; ++j)
    {
      transferByte(static_cast<uint8_t>(pending.topic[j]));
    }
    for (uint8_t j = 0; j < payloadLength; ++j)
    {
      transferByte(static_cast<uint8_t>(pending.payload[j]));
    }
    digitalWrite(SPI_MIRROR_SS_PIN, HIGH);
    spiMirror.endTransaction();

    pending.active = false;
    return;
  }
}
