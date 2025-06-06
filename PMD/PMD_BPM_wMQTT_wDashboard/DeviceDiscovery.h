#ifndef DEVICE_DISCOVERY_H
#define DEVICE_DISCOVERY_H

#include <Arduino.h>

void initDeviceDiscovery(const char* id, const char* type);
void sendDiscoveryBeacon();

#endif
