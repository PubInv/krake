#ifndef __GATEWAY_UTILS__H_
#define __GATEWAY_UTILS__H_

#include <stdint.h>
#include <stddef.h>

// Convert hex string to bytes.  Returns byte count on success, -1 on error.
int  gw_hex_to_bytes(const char *hex, uint8_t *dst, size_t hex_len);

// Derive a 32-byte encryption key from a PSK string via SHA-256.
int  gw_psk_to_key(const char *psk, size_t psk_len, uint8_t *key);

// Verify the HMAC-SHA256 auth signature that is embedded inside every
// encrypted message.
//
// The signed data is the UTF-8 string: "<device_id>:<timestamp>:<method>"
// The key is the 32-byte derived encryption key (not the raw PSK).
//
// Returns 1 if the signature matches, 0 if it does not.
int  gw_verify_auth(const char *device_id,
                    long         timestamp,
                    const char  *method,
                    const char  *auth_hex,   // 64 hex chars (32-byte HMAC)
                    const uint8_t key[32]);

#endif