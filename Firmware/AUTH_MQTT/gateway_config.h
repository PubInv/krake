#ifndef __GATEWAY_CONFIG__H_
#define __GATEWAY_CONFIG__H_

#define MG_DEBUG 1

// ── WiFi ──────────────────────────────────────
#define GW_WIFI_SSID       "WE_NET"
#define GW_WIFI_PASSWORD   "AymanSH@2025_**"

// ── MQTT ──────────────────────────────────────
#define GW_MQTT_BROKER     "broker.hivemq.com"
#define GW_MQTT_PORT       1883
#define GW_GATEWAY_ID      "gateway_01"

// ── MQTT Topics ───────────────────────────────
#define GW_T_GATEWAY_CONNECT    "jrpc/gateway/connect"
#define GW_T_GATEWAY_RX    "jrpc/gateway/rx"

// ── Timing ────────────────────────────────────
#define GW_MQTT_RECONNECT_MS   3000UL

#endif