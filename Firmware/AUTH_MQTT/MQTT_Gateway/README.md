# MQTT_Gateway
### An implementation for adding authentications and authorization for the MQTT messages without depend on the broker side


## Overview

This project is an IoT gateway system with two components:

1. **ESP32 Gateway** (`MQTT_Gateway.ino`) — Runs on an ESP32 microcontroller. Hosts a web dashboard, manages device connections over MQTT, and dispatches JSON-RPC calls using the Mongoose networking library.
2. **Sensor Simulator** — A Python script that simulates IoT sensor devices for testing. Connects to the same MQTT broker and goes through the full authentication flow.

**Architecture:**

```
┌──────────────┐             MQTT broker               ┌──────────────────┐
│  ESP32       │◄─────────────────────────────────────►│  Sensor Device   │
│  Gateway     │   jrpc/connect/request                │  (Python or real │
│              │   jrpc/gateway/rx                     │   ESP32 sensor)  │
│  Port 80:    │   jrpc/devices/{id}/rx                └──────────────────┘
│  Dashboard   │
│  + WebSocket │◄──── Browser (real-time dashboard)
└──────────────┘
```


```
1. Device generates X25519 keypair, publishes PUBLIC key
   in connect request to jrpc/connect/request
         │
         ▼
2. Gateway registers device as PENDING, stores device pubkey
   (appears in dashboard "Pending Approval")
         │
         ▼
3. Admin clicks "Approve" on dashboard
         │
         ▼
4. Gateway generates its own ephemeral X25519 keypair
   Gateway computes: shared = X25519(gateway_prv, device_pub)
   Gateway derives:  hmac_key = HMAC-SHA256("esp32-dashboard-hmac-key", shared)
   Gateway sends its PUBLIC key to device
   Device status → APPROVED
         │
         ▼
5. Device receives gateway pubkey, computes same shared secret:
   shared = X25519(device_prv, gateway_pub)  ← identical result
   hmac_key = HMAC-SHA256("esp32-dashboard-hmac-key-v1", shared)
   Both sides now have the same HMAC key without ever transmitting it
         │
         ▼
6. Device signs each RPC with HMAC-SHA256(hmac_key, canonical_msg)
   canonical_msg = "device_id\nmethod\nrpc_id\nnonce\ntimestamp"
   Nonce is monotonically increasing (prevents replay attacks)
   Device status → AUTHORIZED on first valid HMAC
         │
         ▼
7. If no messages for 60s → device marked OFFLINE
   Admin can Revoke access or Remove device at any time
   Revoke wipes the HMAC key — device must re-do full ECDH
```
### Pre-Shared Key (PSK) — Device Identity Verification

Since the gateway connects to devices via an online MQTT broker, a previously-approved device that reconnects with new keys cannot be automatically distinguished from an attacker spoofing the same device ID. **PSK** solves this by requiring proof-of-identity on every reconnection.

**How it works:**

1. Admin Enter a PSK string of the Device pending to be approved (e.g., `"my-device-secret"`) in the dashboard when first approving a device
2. The device operator configures the same PSK on the physical device
3. Both sides hash the PSK: `SHA-256("my-device-secret")`
4. On every connection request, the device computes:
   ```
   psk_proof = HMAC-SHA256(SHA256(psk), device_id + pubkey_hex)
   ```
5. Gateway computes the same HMAC and verifies with constant-time comparison
6. If verification fails, the connection is rejected

**Security properties of PSK:**
- PSK is **never transmitted** over MQTT — only a cryptographic proof is sent
- Proof is **bound to the public key** — cannot be replayed with a different keypair
- Proof is **bound to the device ID** — cannot be reused for a different device
- PSK is wiped on revoke — a new PSK must be set when re-approving

### Payload Encryption (ChaCha20-Poly1305)

All MQTT payloads are encrypted with ChaCha20-Poly1305 using the **Encrypt-then-MAC** pattern. This prevents eavesdroppers on the MQTT broker from reading sensor data, method names, or any message content.

**Crypto providers:** Cryptographic primitives are provided by:
- **X25519 ECDH** — standalone implementation in `x25519.h` (extracted from Mongoose, public domain)
- **ChaCha20-Poly1305** — standalone implementation in `chacha20.h`/`chacha20.c` (extracted from Mongoose, public domain)
- **SHA-256 / HMAC-SHA256** — Mongoose (always compiled, not gated by TLS setting)
- **Random** — Mongoose's `mg_random()` which uses `esp_random()` on ESP32

**Key derivation** (from the same ECDH shared secret):
```
hmac_key = HMAC-SHA256("esp32-dashboard-hmac-key", shared_secret)
enc_key  = HMAC-SHA256("esp32-dashboard-enc-key",  shared_secret)
```
