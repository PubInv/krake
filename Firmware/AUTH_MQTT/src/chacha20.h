// Standalone ChaCha20-Poly1305 AEAD (RFC 8439)
// Extracted from Mongoose library (Public Domain)
// Original source: https://github.com/cesanta/mongoose

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RFC_8439_TAG_SIZE   16
#define RFC_8439_KEY_SIZE   32
#define RFC_8439_NONCE_SIZE 12

// Encrypt plain_text and append a 16-byte Poly1305 tag.
// cipher_text must be at least plain_text_size + 16 bytes.
// Returns total output size (plain_text_size + 16), or (size_t)-1 on overlap.
size_t chacha20_poly1305_encrypt(
    uint8_t *cipher_text, const uint8_t key[32],
    const uint8_t nonce[12], const uint8_t *ad, size_t ad_size,
    const uint8_t *plain_text, size_t plain_text_size);

// Decrypt cipher_text (which includes 16-byte tag at the end).
// plain_text must be at least cipher_text_size - 16 bytes.
// Returns plaintext size, or (size_t)-1 on overlap.
// NOTE: does not verify the Poly1305 tag (HMAC provides integrity).
size_t chacha20_poly1305_decrypt(
    uint8_t *plain_text, const uint8_t key[32],
    const uint8_t nonce[12],
    const uint8_t *cipher_text, size_t cipher_text_size);

#ifdef __cplusplus
}
#endif
