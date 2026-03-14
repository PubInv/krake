#ifndef __GATEWAY_PRIVATE__H_
#define __GATEWAY_PRIVATE__H_

#include <Arduino.h>
#include "mongoose.h"
#include "../gateway_config.h"
#include "gateway_utils.h"
#include <WiFi.h>

enum DeviceStatus {
  DEV_PENDING,
  DEV_APPROVED,
  DEV_DENIED,
  DEV_OFFLINE
};

struct Device {
  String id;
  String name;
  String type;
  DeviceStatus status;
  uint32_t lastNonce;                // last used counter (for encrypted messages after approval)
  unsigned long firstSeen;
  unsigned long lastSeen;
  int messageCount;

  uint8_t enc_key[32];                // 32‑byte encryption key (derived from PSK, set only after approval)
  bool keySet;

  bool permPing;

  // Pending encrypted request data
  String pending_nonce;
  String pending_cipher;
  bool has_pending;

  Device() : status(DEV_PENDING), lastNonce(0), firstSeen(0), lastSeen(0),
             messageCount(0), permPing(false), keySet(false), has_pending(false) {
    memset(enc_key, 0, sizeof(enc_key));
  }
};

struct DevicePerms {
  bool ping;
};

#endif