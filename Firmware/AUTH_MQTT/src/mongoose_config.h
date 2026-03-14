#pragma once

// ESP32 architecture (uses lwIP sockets via Arduino WiFi)
#define MG_ARCH MG_ARCH_ESP32

// Enable MQTT client
#define MG_ENABLE_MQTT 1

// Disable built-in TCP/IP stack and hardware drivers (ESP32 uses lwIP sockets)
#define MG_ENABLE_TCPIP 0

// Disable Mongoose's built-in TLS/crypto stack (~10,900 lines) to save flash.
// Crypto primitives are now provided by:
//   - x25519.h (standalone X25519 ECDH, extracted from Mongoose)
//   - chacha20.h/chacha20.c (standalone ChaCha20-Poly1305, extracted from Mongoose)
//   - Mongoose's always-compiled mg_sha256/mg_hmac_sha256/mg_random
#define MG_TLS MG_TLS_NONE

// IO buffer size for connections
#define MG_IO_SIZE 2048

// Disable filesystem features we don't need
#define MG_ENABLE_POSIX_FS 0
#define MG_ENABLE_DIRLIST 0

// Disable Mongoose internal logging to save flash (app Serial.printf still works)
#define MG_ENABLE_LOG 0

// Disable unused MD5 (HTTP digest auth)
#define MG_ENABLE_MD5 0
