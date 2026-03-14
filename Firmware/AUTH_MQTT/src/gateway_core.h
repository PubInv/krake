#ifndef __GATEWAY_CORE_H
#define __GATEWAY_CORE_H

#include <Arduino.h>
#include <map>
#include <functional>
#include <LittleFS.h>                   // new
#include "mongoose.h"
#include "gateway_private.h"

class GatewayCore {
public:
  GatewayCore();
  ~GatewayCore();

  void begin();
  void poll();

  Device* getDevice(const String& id);
  const std::map<String, Device>& getAllDevices() const { return m_devices; }
  void approveDevice(const String& id, const DevicePerms& perms, const char* psk = nullptr);
  void denyDevice(const String& id);
  void addDevice(const String& id, const String& name, const String& type);
  bool authorizeDevice(const String& id, const char* psk);
  bool deleteDevice(const String& id);      // remove one device from memory + flash
  void deleteAllDevices();                  // remove every device from memory + flash

  void publishToDevice(const String& deviceId, const char* payload, size_t len);
  void publishToDevice(const String& deviceId, const String& payload) {
    publishToDevice(deviceId, payload.c_str(), payload.length());
  }

  using EventCallback = std::function<void(const String& deviceId, int event)>;
  enum Event { DEVICE_ADDED, DEVICE_UPDATED, DEVICE_REMOVED };
  void onEvent(EventCallback cb) { m_eventCb = cb; }

  struct mg_mgr* getMgr() { return &m_mgr; }

private:
  struct mg_mgr m_mgr;
  struct mg_connection *m_mqttConn;
  struct mg_rpc *m_rpcHead;

  std::map<String, Device> m_devices;
  EventCallback m_eventCb;
  // Preferences prefs;  // removed
  // LittleFS is used directly

  static void mqttEventHandler(struct mg_connection *c, int ev, void *ev_data);
  static void mqttTimerFn(void *arg);
  void handleGatewayConnect(struct mg_str payload);
  void handleGatewayRx(struct mg_str payload);
  void setupRpc();
  
  void gateway_SendApprovalResp(const String& id);
  
  static void rpcPing(struct mg_rpc_req *r);
  static void rpcRequestConnect(struct mg_rpc_req *r);

  void sendError(const String& deviceId, const char* msg);
  void sendEncrypted(const String& deviceId, const uint8_t* plaintext, size_t len);

  void loadDevices();                      // scan /devices directory
  void saveDevice(const Device& dev);       // write to /devices/<id>
  void removeDevice(const String& id);      // delete file
  String safeFilename(const String& id);    // sanitize for filename
};

#endif