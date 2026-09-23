#include "gateway_device.h"

// -------------------------------------------------------------------
// Persistent storage helpers (LittleFS)
// -------------------------------------------------------------------

GatewayDevice::GatewayDevice(){

}

GatewayDevice::~GatewayDevice() {
}
String GatewayDevice::safeFilename(const String& id) {
  String safe = id;
  safe.replace("/", "_");
  safe.replace("\\", "_");
  safe.replace(":", "_");
  safe.replace("*", "_");
  safe.replace("?", "_");
  safe.replace("\"", "_");
  safe.replace("<", "_");
  safe.replace(">", "_");
  safe.replace("|", "_");
  return safe;
}

void GatewayDevice::loadDevices() {
  Serial.println("Loading devices from LittleFS...");
  File root = LittleFS.open("/devices");
  if (!root || !root.isDirectory()) {
    LittleFS.mkdir("/devices");
    Serial.println("Created /devices directory");
    return;
  }

  File file;
  while ((file = root.openNextFile())) {
    if (!file.isDirectory()) {
      String filename = file.name();
      if (filename.startsWith("dev_")) {
        String id = filename.substring(4);
        Serial.printf("Reading device file: %s\n", filename.c_str());
        String jsonStr = file.readString();
        file.close();

        struct mg_str s = mg_str(jsonStr.c_str());
        Device dev;
        char* idStr = mg_json_get_str(s, "$.id");
        if (idStr) dev.id = idStr; else dev.id = id;
        free(idStr);
        char* nameStr = mg_json_get_str(s, "$.name");
        if (nameStr) dev.name = nameStr; else dev.name = id;
        free(nameStr);
        char* typeStr = mg_json_get_str(s, "$.type");
        if (typeStr) dev.type = typeStr; else dev.type = "unknown";
        free(typeStr);
        dev.status = (DeviceStatus)mg_json_get_long(s, "$.status", DEV_PENDING);
        dev.lastNonce = mg_json_get_long(s, "$.lastNonce", 0);
        dev.firstSeen = mg_json_get_long(s, "$.firstSeen", 0);
        dev.lastSeen = mg_json_get_long(s, "$.lastSeen", 0);
        dev.messageCount = mg_json_get_long(s, "$.messageCount", 0);
        bool permPing = false;
        mg_json_get_bool(s, "$.permPing", &permPing);
        dev.permPing = permPing;

        char* keyHex = mg_json_get_str(s, "$.key");
        if (keyHex && strlen(keyHex) == 64) {
          if (gw_hex_to_bytes(keyHex, dev.enc_key, 64) == 32) {
            dev.keySet = true;
          } else {
            Serial.println("Failed to decode key hex");
          }
        }
        free(keyHex);

        dev.has_pending = false;
        devices[dev.id] = dev;
        Serial.printf("Loaded device %s from flash\n", dev.id.c_str());
      }
    }
  }
  root.close();
  Serial.printf("Loaded %d devices from LittleFS\n", devices.size());
}

void GatewayDevice::saveDevice(const Device& dev) {
  char keyHex[65] = "";
  if (dev.keySet) {
    gw_bytes_to_hex(dev.enc_key, 32, keyHex);
  }

  char buf[512];
  int n = mg_snprintf(buf, sizeof(buf),
    "{\"id\":\"%s\",\"name\":\"%s\",\"type\":\"%s\",\"status\":%d,\"lastNonce\":%lu,"
    "\"firstSeen\":%lu,\"lastSeen\":%lu,\"messageCount\":%d,\"permPing\":%s,\"key\":\"%s\"}",
    dev.id.c_str(), dev.name.c_str(), dev.type.c_str(), (int)dev.status,
    (unsigned long)dev.lastNonce, dev.firstSeen, dev.lastSeen, dev.messageCount,
    dev.permPing ? "true" : "false", keyHex);

  String safeId = safeFilename(dev.id);
  String path = "/devices/dev_" + safeId;
  File f = LittleFS.open(path, "w");
  if (f) {
    f.print(buf);
    f.close();
    Serial.printf("Saved device %s to flash\n", dev.id.c_str());
  } else {
    Serial.printf("Failed to save device %s\n", dev.id.c_str());
  }
}

void GatewayDevice::removeDevice(const String& id) {
  String safeId = safeFilename(id);
  String path = "/devices/dev_" + safeId;
  if (LittleFS.remove(path)) {
    Serial.printf("Removed device %s from flash\n", id.c_str());
  } else {
    Serial.printf("Failed to remove device %s\n", id.c_str());
  }
}
