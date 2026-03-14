#include "gateway_utils.h"
#include "mongoose.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
int gw_hex_to_bytes(const char *hex, uint8_t *dst, size_t hex_len) {
  if (hex_len % 2 != 0) return -1;
  size_t byte_len = hex_len / 2;
  for (size_t i = 0; i < byte_len; i++) {
    unsigned int b;
    if (sscanf(hex + i * 2, "%2x", &b) != 1) return -1;
    dst[i] = (uint8_t)b;
  }
  return (int)byte_len;
}

// ---------------------------------------------------------------------------
int gw_psk_to_key(const char *psk, size_t psk_len, uint8_t *key) {
  mg_sha256(key, (uint8_t*)psk, psk_len);
  return 0;
}

// ---------------------------------------------------------------------------
// gw_verify_auth
//
// Signed message: "<device_id>:<timestamp>:<method>"   (plain ASCII string)
// Algorithm     : HMAC-SHA256 with the 32-byte derived encryption key
// Expected value: auth_hex (64 hex chars = 32 bytes)
//
// Returns 1 on match, 0 on any mismatch or error.
// ---------------------------------------------------------------------------
int gw_verify_auth(const char  *device_id,
                   long         timestamp,
                   const char  *method,
                   const char  *auth_hex,
                   const uint8_t key[32]) {
  if (!device_id || !method || !auth_hex || !key) return 0;
  if (strlen(auth_hex) != 64) return 0;

  // Build the canonical signed string: "<device_id>:<timestamp>:<method>"
  char msg[256];
  int  msg_len = snprintf(msg, sizeof(msg), "%s:%ld:%s",
                          device_id, timestamp, method);
  if (msg_len <= 0 || msg_len >= (int)sizeof(msg)) return 0;

  // Compute HMAC-SHA256
  uint8_t computed[32];
  mg_hmac_sha256(computed, (uint8_t*)key, 32, (uint8_t*)msg, (size_t)msg_len);

  // Decode the expected value from hex
  uint8_t expected[32];
  if (gw_hex_to_bytes(auth_hex, expected, 64) != 32) return 0;

  // Constant-time comparison to prevent timing attacks
  uint8_t diff = 0;
  for (int i = 0; i < 32; i++) diff |= computed[i] ^ expected[i];
  return (diff == 0) ? 1 : 0;
}