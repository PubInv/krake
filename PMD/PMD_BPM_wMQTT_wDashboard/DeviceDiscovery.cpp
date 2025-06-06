#include "DeviceDiscovery.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>

WiFiUDP discoveryUdp;
const IPAddress multicastIP(239, 1, 1, 1);
const int multicastPort = 12345;

String deviceID = "krake-001";
String deviceType = "Krake";

void initDeviceDiscovery(const char* id, const char* type) {
  deviceID = id;
  deviceType = type;

  if (!MDNS.begin(deviceID.c_str())) {
    Serial.println("❌ mDNS failed");
  } else {
    Serial.println("✅ mDNS started");
  }

  // ✅ Correct line: just use standard begin
  if (discoveryUdp.begin(multicastPort)) {
    Serial.printf("✅ UDP started on port %d\n", multicastPort);
  } else {
    Serial.println("❌ Failed to start UDP");
  }
}

void sendDiscoveryBeacon() {
  String message = deviceType + "|" + deviceID + "|" + WiFi.localIP().toString();

  // ✅ Correct line for sending UDP multicast
  discoveryUdp.beginPacket(multicastIP, multicastPort);
  discoveryUdp.print(message);
  discoveryUdp.endPacket();

  Serial.println("📣 Beacon sent: " + message);
}
