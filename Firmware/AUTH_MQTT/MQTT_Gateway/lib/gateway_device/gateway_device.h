#ifndef __GATEWAY_DEVICE_H
#define __GATEWAY_DEVICE_H
#include <Arduino.h>
#include <LittleFS.h>                   // new
#include "gateway_private.h"
#include <map>
#include"gateway_utils.h"

class GatewayDevice {
public:
  GatewayDevice();
  ~GatewayDevice();

    void loadDevices();                      // scan /devices directory
  void saveDevice(const Device& dev);       // write to /devices/<id>
  void removeDevice(const String& id);      // delete file
  String safeFilename(const String& id);    // sanitize for filename
  std::map<String, Device> devices;

};
#endif