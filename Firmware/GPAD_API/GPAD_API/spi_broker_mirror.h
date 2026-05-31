#ifndef SPI_BROKER_MIRROR_H
#define SPI_BROKER_MIRROR_H

#include <Arduino.h>

// Broker mirror frame, transmitted MSB-first over the KRAKE VSPI controller:
//   "KRK1", retain byte, topic length byte, payload length byte, topic, payload.
// Topic and payload bytes are the same bytes queued for MQTT publication.
void initializeSpiBrokerMirror();
bool queueSpiBrokerMirror(const char *topic, const char *payload, bool retain = false);
void serviceSpiBrokerMirror();

#endif
