// Standalone X25519 Diffie-Hellman implementation
// Extracted from Mongoose library (Public Domain)
// Original source: https://github.com/cesanta/mongoose

#pragma once

#include <stdint.h>

#ifndef X25519_BYTES
#define X25519_BYTES 32
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t X25519_BASE_POINT[X25519_BYTES];

int x25519(uint8_t out[X25519_BYTES],
           const uint8_t scalar[X25519_BYTES],
           const uint8_t x1[X25519_BYTES], int clamp);

#ifdef __cplusplus
}
#endif
