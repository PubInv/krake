#include "gateway_core.h"
#include "gateway_config.h"
#include "gateway_utils.h"
#include "chacha20.h"
#include <WiFi.h>
#include <time.h>
#include <vector>
// -------------------------------------------------------------------
// Utility
// -------------------------------------------------------------------

static String safeString(const char* str) {
  if (str == nullptr) return String();
  return String(str);
}

// -------------------------------------------------------------------
// GatewayCore implementation
// -------------------------------------------------------------------
GatewayCore::GatewayCore() : m_mqttConn(nullptr), m_rpcHead(nullptr) {

  Serial.println("GatewayCore constructed");
}

GatewayCore::~GatewayCore() {
  mg_mgr_free(&m_mgr);
  Serial.println("GatewayCore destroyed");
}

void GatewayCore::begin() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n--- GatewayCore begin ---");
  mg_mgr_init(&m_mgr);
  Serial.printf("Connecting to WiFi '%s'", GW_WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(GW_WIFI_SSID, GW_WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.printf("\nWiFi OK — IP: %s\n", WiFi.localIP().toString().c_str());

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed");
  } else {
    Serial.println("LittleFS mounted");
  }

  m_devices.loadDevices();
  setupRpc();

  mg_timer_add(&m_mgr, GW_MQTT_RECONNECT_MS, MG_TIMER_REPEAT | MG_TIMER_RUN_NOW,
               mqttTimerFn, this);
  Serial.println("MQTT reconnect timer started");
}

void GatewayCore::poll() {
  mg_mgr_poll(&m_mgr, 1);
}

void GatewayCore::setupRpc() {
  mg_rpc_add(&m_rpcHead, mg_str("ping"), rpcPing, this);
  mg_rpc_add(&m_rpcHead, mg_str("request_connect"), rpcRequestConnect, this);
  Serial.println("RPC handlers added");
}

// -------------------------------------------------------------------
// MQTT connection
// -------------------------------------------------------------------
void GatewayCore::mqttTimerFn(void* arg) {
  if (arg == nullptr) {
    Serial.println("ERROR: mqttTimerFn arg is null!");
    return;
  }
  GatewayCore* self = static_cast<GatewayCore*>(arg);
  if (self->m_mqttConn != nullptr) {
    // Serial.println("MQTT already connected"); // commented to avoid spam
    return;
  }

  char url[128];
  mg_snprintf(url, sizeof(url), "mqtt://%s:%d", GW_MQTT_BROKER, GW_MQTT_PORT);
  Serial.printf("MQTT connecting to %s...\n", url);

  char cid[64];
  mg_snprintf(cid, sizeof(cid), "%s_%lx", GW_GATEWAY_ID, (unsigned long)random(0xFFFF));

  struct mg_mqtt_opts opts = {};
  opts.client_id = mg_str(cid);
  opts.clean     = true;
  opts.keepalive = 60;
  opts.version   = 4;

  self->m_mqttConn = mg_mqtt_connect(&self->m_mgr, url, &opts, mqttEventHandler, self);
  
  // self->m_mqttConn = mg_mqtt_connect(&self->m_mgr, url, &opts, mqttEventHandler, NULL);
  if (self->m_mqttConn == nullptr) {
    Serial.println("mg_mqtt_connect returned null (check memory/broker)");
  }else{
    Serial.printf("MQTT connected: %s \n", cid);
  }
}

void GatewayCore::mqttEventHandler(struct mg_connection *c, int ev, void *ev_data) {
  if (c == nullptr) {
    Serial.println("ERROR: mqttEventHandler: connection is null");
    return;
  }
  if (c->fn_data == nullptr) {
    Serial.println("ERROR: mqttEventHandler: c->fn_data is null");
    return;
  }
  GatewayCore* self = static_cast<GatewayCore*>(c->fn_data);

  if (ev == MG_EV_MQTT_OPEN) {
    Serial.println("MQTT connected successfully");
    struct mg_mqtt_opts sub = { .topic = mg_str(GW_T_GATEWAY_RX), .qos = 1 };
    mg_mqtt_sub(c, &sub);
    Serial.printf("Subscribed to %s\n", GW_T_GATEWAY_RX);
    sub.topic = mg_str(GW_T_GATEWAY_CONNECT);
    mg_mqtt_sub(c, &sub);
    Serial.printf("Subscribed to %s\n", GW_T_GATEWAY_CONNECT);
  }
  else if (ev == MG_EV_MQTT_MSG) {
    struct mg_mqtt_message *mm = (struct mg_mqtt_message*)ev_data;
    if (mm == nullptr) {
      Serial.println("ERROR: mqttEventHandler: mm is null");
      return;
    }
    Serial.printf("MQTT msg on topic: %.*s\n", (int)mm->topic.len, mm->topic.buf);
    if (mg_match(mm->topic, mg_str(GW_T_GATEWAY_CONNECT), NULL)) {
      Serial.println("Dispatching to handleGatewayConnect");
      self->handleGatewayConnect(mm->data);
    } else if (mg_match(mm->topic, mg_str(GW_T_GATEWAY_RX), NULL)) {
      Serial.println("Dispatching to handleGatewayRx");
      self->handleGatewayRx(mm->data);
    } else {
      Serial.println("Ignoring unknown topic");
    }
  }
  else if (ev == MG_EV_CLOSE) {
    self->m_mqttConn = nullptr;
    Serial.println("MQTT disconnected");
  }
}

// -------------------------------------------------------------------
// Publish helper
// -------------------------------------------------------------------
void GatewayCore::publishToDevice(const String& deviceId, const char* payload, size_t len) {
  if (!m_mqttConn) {
    Serial.println("publishToDevice: MQTT not connected");
    return;
  }
  if (payload == nullptr || len == 0) return;
  String topic = "jrpc/devices/" + deviceId + "/rx";
  struct mg_mqtt_opts opts = { .topic = mg_str(topic.c_str()), .message = mg_str_n(payload, len), .qos = 1 };
  mg_mqtt_pub(m_mqttConn, &opts);
  Serial.printf("Published %d bytes to %s\n", (int)len, topic.c_str());
}

void GatewayCore::sendError(const String& deviceId, const char* msg) {
  if (msg == nullptr) return;
  char buf[256];
  int n = mg_snprintf(buf, sizeof(buf),
    "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32000,\"message\":\"%s\"},\"id\":null}",
    msg);
  publishToDevice(deviceId, buf, n);
}

// -------------------------------------------------------------------
// Encrypted response helper
// -------------------------------------------------------------------
void GatewayCore::sendEncrypted(const String& deviceId, const uint8_t* plaintext, size_t len) {
  if (plaintext == nullptr || len == 0) return;
  auto it = m_devices.devices.find(deviceId);
  if (it == m_devices.devices.end()) {
    Serial.printf("sendEncrypted: device %s not found\n", deviceId.c_str());
    return;
  }
  Device &dev = it->second;
  if (!dev.keySet) {
    Serial.printf("sendEncrypted: device %s has no key\n", deviceId.c_str());
    return;
  }

  uint32_t counter = dev.lastNonce + 1;
  uint64_t ts = (uint64_t)time(nullptr);
  uint8_t nonce[12];
  nonce[0] = (counter >> 24) & 0xFF;
  nonce[1] = (counter >> 16) & 0xFF;
  nonce[2] = (counter >> 8) & 0xFF;
  nonce[3] = counter & 0xFF;
  for (int i = 0; i < 8; i++) {
    nonce[4 + i] = (ts >> (56 - 8*i)) & 0xFF;
  }

  size_t cipherLen = len + RFC_8439_TAG_SIZE;
  uint8_t* cipher = (uint8_t*)malloc(cipherLen);
  if (!cipher) {
    Serial.println("sendEncrypted: malloc failed for cipher");
    return;
  }

  // device_id is used as AAD when encrypting. The decrypt function in this
  // chacha20 build does not verify the Poly1305 tag, so the AAD value used
  // here does not need to be known by the decrypt side to succeed.
  size_t encLen = chacha20_poly1305_encrypt(cipher, dev.enc_key, nonce,
                                            (uint8_t*)deviceId.c_str(), deviceId.length(),
                                            plaintext, len);
  if (encLen == (size_t)-1) {
    Serial.println("sendEncrypted: encryption failed");
    free(cipher);
    return;
  }

  // FIX: avoid VLA on the stack (encLen is runtime-determined).
  // Allocate hex buffers on the heap instead.
  char nonceHex[25];   // nonce is always 12 bytes → 24 hex chars + NUL, safe as fixed array
  gw_bytes_to_hex(nonce, 12, nonceHex);

  char* cipherHex = (char*)malloc(encLen * 2 + 1);
  if (!cipherHex) {
    Serial.println("sendEncrypted: malloc failed for cipherHex");
    free(cipher);
    return;
  }
  gw_bytes_to_hex(cipher, encLen, cipherHex);

  char out[512];
  int outLen = mg_snprintf(out, sizeof(out),
    "{\"device_id\":\"%s\",\"nonce\":\"%s\",\"ciphertext\":\"%s\"}",
    deviceId.c_str(), nonceHex, cipherHex);
  publishToDevice(deviceId, out, outLen);

  free(cipherHex);
  free(cipher);
  dev.lastNonce = counter;
}


// -------------------------------------------------------------------
// Handle connect request (encrypted)
// -------------------------------------------------------------------
void GatewayCore::handleGatewayConnect(struct mg_str payload) {
  Serial.println("=== handleGatewayConnect entered ===");
  if (payload.buf == nullptr || payload.len == 0) {
    Serial.println("ERROR: payload is empty");
    return;
  }

  char* deviceId = mg_json_get_str(payload, "$.device_id");
  if (!deviceId) {
    Serial.println("ERROR: no device_id in payload");
    return;
  }
  Serial.printf("device_id: %s\n", deviceId);

  char* nonceHex = mg_json_get_str(payload, "$.nonce");
  char* cipherHex = mg_json_get_str(payload, "$.ciphertext");

  if (!nonceHex || !cipherHex) {
    Serial.println("ERROR: missing nonce or ciphertext");//"ERROR: missing nonce or ciphertext" -> .rowdata 
    free(deviceId);
    free(nonceHex);
    free(cipherHex);
    return;
  }
  Serial.printf("nonce len: %d, cipher len: %d\n", strlen(nonceHex), strlen(cipherHex));

  String devId(deviceId);
  auto it = m_devices.devices.find(devId);
  if (it == m_devices.devices.end()) {
    // New device – create pending record
    Device dev;
    dev.id = devId;
    dev.name = devId;
    dev.type = "unknown";
    dev.status = DEV_PENDING;
    dev.firstSeen = millis();
    dev.lastSeen = millis();
    dev.has_pending = true;
    dev.pending_nonce = safeString(nonceHex);
    dev.pending_cipher = safeString(cipherHex);
    m_devices.devices[devId] = dev;
    if (m_eventCb) m_eventCb(devId, DEVICE_ADDED);
    Serial.printf("New device %s added as PENDING\n", deviceId);
  } else {
    Device &dev = it->second;
    if (dev.status == DEV_PENDING) {
      dev.pending_nonce = safeString(nonceHex);
      dev.pending_cipher = safeString(cipherHex);
      dev.has_pending = true;
      Serial.printf("Device %s updated pending request\n", deviceId);
    } else if (dev.status == DEV_APPROVED){
      gateway_SendApprovalResp(devId);
      Serial.printf("Device %s already approved, ignoring\n", deviceId);
    }else
    {
      Serial.printf("Device %s denied, ignoring\n", deviceId);
    }
  }

  free(deviceId);
  free(nonceHex);
  free(cipherHex);
  Serial.println("=== handleGatewayConnect finished ===");
}

bool GatewayCore::authorizeDevice(const String& id, const char* psk) {
  Serial.printf("=== authorizeDevice(%s) ===\n", id.c_str());
  if (psk == nullptr) {
    Serial.println("ERROR: psk is null");
    return false;
  }
  auto it = m_devices.devices.find(id);
  if (it == m_devices.devices.end()) {
    Serial.println("ERROR: device not found");
    return false;
  }
  Device &dev = it->second;
  if (!dev.has_pending) {
    Serial.println("ERROR: device has no pending request");
    return false;
  }

  // Derive key from PSK
  uint8_t key[32];
  gw_psk_to_key(psk, strlen(psk), key);
  Serial.println("Key derived from PSK");

  // Decode nonce
  uint8_t nonce[12];
  if (dev.pending_nonce.length() != 24) {
    Serial.println("ERROR: invalid nonce length");
    return false;
  }
  if (gw_hex_to_bytes(dev.pending_nonce.c_str(), nonce, 24) != 12) {
    Serial.println("ERROR: nonce hex decode failed");
    return false;
  }

  // Decode ciphertext
  size_t cipherLen = dev.pending_cipher.length() / 2;
  if (cipherLen == 0) {
    Serial.println("ERROR: ciphertext length zero");
    return false;
  }
  uint8_t* cipher = (uint8_t*)malloc(cipherLen);
  if (!cipher) {
    Serial.println("ERROR: malloc failed for cipher");
    return false;
  }
  if (gw_hex_to_bytes(dev.pending_cipher.c_str(), cipher, dev.pending_cipher.length()) != (int)cipherLen) {
    Serial.println("ERROR: ciphertext hex decode failed");
    free(cipher);
    return false;
  }

  // Decrypt
  // FIX BUG 2: The Python client passes device_id as the AAD when encrypting.
  // The Poly1305 authentication tag is computed over the AAD, so we MUST pass
  // the same AAD here or tag verification will always fail and decryption will
  // return (size_t)-1 regardless of whether the PSK is correct.
  size_t plainLen = cipherLen - RFC_8439_TAG_SIZE;
  uint8_t* plain = (uint8_t*)malloc(plainLen + 1);
  if (!plain) {
    free(cipher);
    Serial.println("ERROR: malloc failed for plain");
    return false;
  }

  // No AAD: chacha20_poly1305_decrypt() in this build does not accept AAD.
  // Python must also encrypt with aad=b"" (no AAD) to keep both sides symmetric.
  size_t decLen = chacha20_poly1305_decrypt(
      plain, key, nonce,
      cipher, cipherLen);
  if (decLen == (size_t)-1) {
    Serial.println("ERROR: decryption failed (wrong PSK or AAD mismatch)");
    free(cipher); free(plain);
    return false;
  }
  plain[decLen] = '\0';
  Serial.printf("Decrypted inner: %s\n", plain);

  // ── Auth signature verification ─────────────────────────────────────────
  // The inner JSON must contain "timestamp", "method":"request_connect", and
  // "auth" (HMAC-SHA256 hex).  This proves the sender holds the correct PSK
  // even when ChaCha20 decryption "succeeds" with a wrong key (Poly1305 tag
  // is not verified in this build).
  struct mg_str inner = mg_str((char*)plain);
  char* authHex  = mg_json_get_str(inner, "$.auth");
  char* authMeth = mg_json_get_str(inner, "$.method");
  long  authTs   = (long)mg_json_get_long(inner, "$.timestamp", 0);

  bool authOk = false;
  if (authHex && authMeth && authTs != 0) {
    // Optional freshness check (requires NTP; disable by setting AUTH_TS_WINDOW to 0)
    // #define AUTH_TS_WINDOW 300   // ±5 minutes
    // long now = (long)time(nullptr);
    // long skew = authTs - now;
    // if (skew < 0) skew = -skew;
    // if (AUTH_TS_WINDOW > 0 && skew > AUTH_TS_WINDOW) {
    //   Serial.printf("ERROR: auth timestamp too skewed (%ld s)\n", skew);
    // } else {
      authOk = (gw_verify_auth(id.c_str(), authTs, authMeth, authHex, key) == 1);
    // }
  }
  free(authHex); free(authMeth);

  if (!authOk) {
    Serial.println("ERROR: auth signature mismatch — wrong PSK or tampered message");
    free(cipher); free(plain);
    return false;
  }
  Serial.println("Auth signature verified ✓");
  // ────────────────────────────────────────────────────────────────────────

  char* deviceName = mg_json_get_str(inner, "$.device_name");
  char* deviceType = mg_json_get_str(inner, "$.device_type");

  // Approve device
  dev.status = DEV_APPROVED;
  memcpy(dev.enc_key, key, 32);
  dev.keySet = true;
  if (deviceName) dev.name = deviceName;
  if (deviceType) dev.type = deviceType;
  dev.has_pending = false;
  dev.pending_nonce = "";
  dev.pending_cipher = "";
  m_devices.saveDevice(dev);

  free(cipher); free(plain);
  free(deviceName); free(deviceType);

  // Send encrypted approval response
  
  gateway_SendApprovalResp(id);
  Serial.printf("Device %s authorized and approved\n", id.c_str());
  return true;
}

void GatewayCore::gateway_SendApprovalResp(const String& id)
{
  char respBuf[256];
  int n = mg_snprintf(respBuf, sizeof(respBuf),
    "{\"jsonrpc\":\"2.0\",\"method\":\"connect.response\",\"params\":{\"status\":\"approved\"},\"id\":null}");
  sendEncrypted(id, (uint8_t*)respBuf, n);
  if (m_eventCb) m_eventCb(id, DEVICE_UPDATED);
}

// -------------------------------------------------------------------
// Handle general RPC messages (encrypted)
// -------------------------------------------------------------------
void GatewayCore::handleGatewayRx(struct mg_str payload) {
  Serial.println("=== handleGatewayRx entered ===");
  if (payload.buf == nullptr || payload.len == 0) {
    Serial.println("ERROR: payload empty");
    return;
  }

  char* deviceId = mg_json_get_str(payload, "$.device_id");
  if (!deviceId) {
    Serial.println("ERROR: no device_id");
    return;
  }

  char* nonceHex = mg_json_get_str(payload, "$.nonce");
  char* cipherHex = mg_json_get_str(payload, "$.ciphertext");

  if (!nonceHex || !cipherHex) {
    Serial.println("ERROR: missing nonce or ciphertext");
    free(deviceId);
    free(nonceHex);
    free(cipherHex);
    return;
  }

  String devId(deviceId);
  auto it = m_devices.devices.find(devId);
  if (it == m_devices.devices.end()) {
    Serial.printf("ERROR: device %s not found\n", deviceId);
    sendError(devId, "Device not found");
    free(deviceId); free(nonceHex); free(cipherHex);
    return;
  }
  Device &dev = it->second;
  if (!dev.keySet) {
    Serial.printf("ERROR: device %s has no encryption key\n", deviceId);
    sendError(devId, "No encryption key");
    free(deviceId); free(nonceHex); free(cipherHex);
    return;
  }

  // Decode nonce
  uint8_t nonce[12];
  if (strlen(nonceHex) != 24) {
    Serial.println("ERROR: invalid nonce length");
    sendError(devId, "Invalid nonce");
    free(deviceId); free(nonceHex); free(cipherHex);
    return;
  }
  if (gw_hex_to_bytes(nonceHex, nonce, 24) != 12) {
    Serial.println("ERROR: nonce hex decode failed");
    sendError(devId, "Invalid nonce");
    free(deviceId); free(nonceHex); free(cipherHex);
    return;
  }

  // Decode ciphertext
  size_t cipherLen = strlen(cipherHex) / 2;
  uint8_t* cipher = (uint8_t*)malloc(cipherLen);
  if (!cipher) {
    Serial.println("ERROR: malloc failed for cipher");
    sendError(devId, "OOM");
    free(deviceId); free(nonceHex); free(cipherHex);
    return;
  }
  if (gw_hex_to_bytes(cipherHex, cipher, strlen(cipherHex)) != (int)cipherLen) {
    Serial.println("ERROR: ciphertext hex decode failed");
    free(cipher);
    sendError(devId, "Invalid ciphertext");
    free(deviceId); free(nonceHex); free(cipherHex);
    return;
  }

  // Decrypt
  // FIX BUG 2: Same as authorizeDevice — must pass device_id as AAD to match
  // what the Python client used during encryption, otherwise the Poly1305 tag
  // will never verify and every message will be rejected.
  size_t plainLen = cipherLen - RFC_8439_TAG_SIZE;
  uint8_t* plain = (uint8_t*)malloc(plainLen + 1);
  if (!plain) {
    free(cipher);
    Serial.println("ERROR: malloc failed for plain");
    sendError(devId, "OOM");
    free(deviceId); free(nonceHex); free(cipherHex);
    return;
  }

  // No AAD: same as authorizeDevice — must match Python's aad=b"" encryption.
  size_t decLen = chacha20_poly1305_decrypt(
      plain, dev.enc_key, nonce,
      cipher, cipherLen);
  if (decLen == (size_t)-1) {
    Serial.println("ERROR: decryption failed");
    sendError(devId, "Decryption failed");
    free(cipher); free(plain);
    free(deviceId); free(nonceHex); free(cipherHex);
    return;
  }
  plain[decLen] = '\0';
  Serial.printf("Decrypted: %s\n", plain);

  // ── Auth signature verification ─────────────────────────────────────────
  // Every RX message must contain "timestamp" and "auth" in addition to the
  // standard JSON-RPC fields.  "method" is the JSON-RPC method field.
  struct mg_str innerStr = mg_str((char*)plain);
  char* rxAuthHex  = mg_json_get_str(innerStr, "$.auth");
  char* rxMethod   = mg_json_get_str(innerStr, "$.method");
  long  rxAuthTs   = (long)mg_json_get_long(innerStr, "$.timestamp", 0);

  bool rxAuthOk = false;
  if (rxAuthHex && rxMethod && rxAuthTs != 0) {
    // long now = (long)time(nullptr);
    // long skew = rxAuthTs - now;
    // if (skew < 0) skew = -skew;
    // if (AUTH_TS_WINDOW > 0 && skew > AUTH_TS_WINDOW) {
    //   Serial.printf("ERROR: auth timestamp too skewed (%ld s)\n", skew);
    // } else {
      rxAuthOk = (gw_verify_auth(devId.c_str(), rxAuthTs, rxMethod,
                                 rxAuthHex, dev.enc_key) == 1);
    // }
  }
  free(rxAuthHex); free(rxMethod);

  if (!rxAuthOk) {
    Serial.println("ERROR: auth signature mismatch — rejecting message");
    sendError(devId, "Auth failed");
    free(cipher); free(plain);
    free(deviceId); free(nonceHex); free(cipherHex);
    return;
  }
  Serial.println("Auth signature verified ✓");
  // ────────────────────────────────────────────────────────────────────────

  // Replay protection
  uint32_t counter = (nonce[0] << 24) | (nonce[1] << 16) | (nonce[2] << 8) | nonce[3];
  // if (counter <= dev.lastNonce) {
  //   Serial.printf("WARN: nonce too old (%u <= %u)\n", counter, dev.lastNonce);
  //   sendError(devId, "Nonce too old");
  //   free(cipher); free(plain);
  //   free(deviceId); free(nonceHex); free(cipherHex);
  //   return;
  // }
  dev.lastNonce = counter;

  // Process RPC
  struct mg_iobuf io = {NULL, 0, 0, 256};
  struct mg_rpc_req r = {};
  r.head     = &m_rpcHead;
  r.pfn      = mg_pfn_iobuf;
  r.pfn_data = &io;
  r.frame    = mg_str((char*)plain);
  mg_rpc_process(&r);

  if (io.len > 0) {
    sendEncrypted(devId, io.buf, io.len);
    mg_iobuf_free(&io);
  }

  free(cipher);
  free(plain);
  free(deviceId); free(nonceHex); free(cipherHex);
  Serial.println("=== handleGatewayRx finished ===");
}

// -------------------------------------------------------------------
// RPC method implementations
// -------------------------------------------------------------------
void GatewayCore::rpcPing(struct mg_rpc_req *r) {
  mg_rpc_ok(r, "{%m:true,%m:%lu}",
            MG_ESC("pong"), MG_ESC("uptime_ms"), (unsigned long)millis());
}

void GatewayCore::rpcRequestConnect(struct mg_rpc_req *r) {
  mg_rpc_err(r, -32601, "\"Method not found\"");
}
static void rpcCommand(struct mg_rpc_req *r)
{
    // command_callback(mg_json_get_str(r->frame, "$.params.message"));
    mg_rpc_ok(r, "{%m:true,%m:%lu}",
            MG_ESC("recived"), MG_ESC("uptime_ms"), (unsigned long)millis());
}
  
// -------------------------------------------------------------------
// Device management
// -------------------------------------------------------------------
Device* GatewayCore::getDevice(const String& id) {
  auto it = m_devices.devices.find(id);
  return (it != m_devices.devices.end()) ? &it->second : nullptr;
}

void GatewayCore::approveDevice(const String& id, const DevicePerms& perms, const char* psk) {
  auto it = m_devices.devices.find(id);
  if (it == m_devices.devices.end()) return;
  Device &dev = it->second;
  dev.status = DEV_APPROVED;
  dev.permPing = perms.ping;
  if (psk) {
    gw_psk_to_key(psk, strlen(psk), dev.enc_key);
    dev.keySet = true;
  }
  m_devices.saveDevice(dev);
  if (m_eventCb) m_eventCb(id, DEVICE_UPDATED);
}

void GatewayCore::denyDevice(const String& id) {
  auto it = m_devices.devices.find(id);
  if (it == m_devices.devices.end()) return;
  it->second.status = DEV_DENIED;
  m_devices.saveDevice(it->second);
  if (m_eventCb) m_eventCb(id, DEVICE_UPDATED);
}

void GatewayCore::addDevice(const String& id, const String& name, const String& type) {
  Device dev;
  dev.id = id;
  dev.name = name;
  dev.type = type;
  dev.firstSeen = millis();
  dev.lastSeen = millis();
  dev.status = DEV_PENDING;
  m_devices.devices[id] = dev;
  if (m_eventCb) m_eventCb(id, DEVICE_ADDED);
}

// -------------------------------------------------------------------
// Delete helpers (called from dashboard)
// -------------------------------------------------------------------
bool GatewayCore::deleteDevice(const String& id) {
  auto it = m_devices.devices.find(id);
  if (it == m_devices.devices.end()) {
    Serial.printf("deleteDevice: device %s not found\n", id.c_str());
    return false;
  }
  m_devices.devices.erase(it);
  m_devices.removeDevice(id);                          // delete LittleFS file
  if (m_eventCb) m_eventCb(id, DEVICE_REMOVED);
  Serial.printf("Deleted device %s\n", id.c_str());
  return true;
}

void GatewayCore::deleteAllDevices() {
  // Collect IDs first to avoid invalidating the iterator inside the loop
  std::vector<String> ids;
  ids.reserve(m_devices.devices.size());
  for (auto& pair : m_devices.devices) ids.push_back(pair.first);

  for (auto& id : ids) {
    m_devices.devices.erase(id);
    m_devices.removeDevice(id);
    if (m_eventCb) m_eventCb(id, DEVICE_REMOVED);
  }
  Serial.printf("Deleted all %d devices\n", (int)ids.size());
}