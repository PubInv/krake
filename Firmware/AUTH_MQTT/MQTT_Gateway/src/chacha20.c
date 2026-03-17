// Standalone ChaCha20-Poly1305 AEAD (RFC 8439)
// Extracted from Mongoose library (Public Domain)
// Original: https://github.com/cesanta/mongoose
// Contains: chacha-portable + poly1305-donna (32-bit path) + RFC 8439 AEAD

#include "chacha20.h"
#include <string.h>

// ─────────────────────────────────────────────
//  ChaCha20 stream cipher
// ─────────────────────────────────────────────

#define CHACHA20_KEY_SIZE   32
#define CHACHA20_NONCE_SIZE 12
#define CHACHA20_STATE_WORDS 16
#define CHACHA20_BLOCK_SIZE (CHACHA20_STATE_WORDS * sizeof(uint32_t))

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && \
    __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define HAVE_LITTLE_ENDIAN 1
#elif defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) ||                 \
    defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) || \
    defined(__MIPSEL) || defined(__MIPSEL__) || defined(__XTENSA_EL__) || \
    defined(__AVR__)
#define HAVE_LITTLE_ENDIAN 1
#endif

#ifdef HAVE_LITTLE_ENDIAN
#define store_32_le(target, source) memcpy(&(target), source, sizeof(uint32_t))
#else
#define store_32_le(target, source)                                 \
  target = (uint32_t)(source)[0] | ((uint32_t)(source)[1]) << 8 |  \
           ((uint32_t)(source)[2]) << 16 | ((uint32_t)(source)[3]) << 24
#endif

static void initialize_state(uint32_t state[CHACHA20_STATE_WORDS],
                             const uint8_t key[CHACHA20_KEY_SIZE],
                             const uint8_t nonce[CHACHA20_NONCE_SIZE],
                             uint32_t counter) {
  state[0] = 0x61707865;
  state[1] = 0x3320646e;
  state[2] = 0x79622d32;
  state[3] = 0x6b206574;
  store_32_le(state[4], key);
  store_32_le(state[5], key + 4);
  store_32_le(state[6], key + 8);
  store_32_le(state[7], key + 12);
  store_32_le(state[8], key + 16);
  store_32_le(state[9], key + 20);
  store_32_le(state[10], key + 24);
  store_32_le(state[11], key + 28);
  state[12] = counter;
  store_32_le(state[13], nonce);
  store_32_le(state[14], nonce + 4);
  store_32_le(state[15], nonce + 8);
}

#define rotl32a(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define Qround(a, b, c, d) \
  a += b; d ^= a; d = rotl32a(d, 16); \
  c += d; b ^= c; b = rotl32a(b, 12); \
  a += b; d ^= a; d = rotl32a(d, 8);  \
  c += d; b ^= c; b = rotl32a(b, 7);

#define TIMES16(x) \
  x(0) x(1) x(2) x(3) x(4) x(5) x(6) x(7) \
  x(8) x(9) x(10) x(11) x(12) x(13) x(14) x(15)

static void core_block(const uint32_t *start, uint32_t *output) {
  int i;
#define __LV(i) uint32_t __t##i = start[i];
  TIMES16(__LV)
#define __Q(a, b, c, d) Qround(__t##a, __t##b, __t##c, __t##d)
  for (i = 0; i < 10; i++) {
    __Q(0, 4, 8, 12); __Q(1, 5, 9, 13);
    __Q(2, 6, 10, 14); __Q(3, 7, 11, 15);
    __Q(0, 5, 10, 15); __Q(1, 6, 11, 12);
    __Q(2, 7, 8, 13); __Q(3, 4, 9, 14);
  }
#define __FIN(i) output[i] = start[i] + __t##i;
  TIMES16(__FIN)
}

#define U8(x) ((uint8_t)((x) & 0xFF))

#ifdef HAVE_LITTLE_ENDIAN
#define xor32_le(dst, src, pad)            \
  uint32_t __value;                        \
  memcpy(&__value, src, sizeof(uint32_t)); \
  __value ^= *(pad);                       \
  memcpy(dst, &__value, sizeof(uint32_t));
#else
#define xor32_le(dst, src, pad)           \
  (dst)[0] = (src)[0] ^ U8(*(pad));       \
  (dst)[1] = (src)[1] ^ U8(*(pad) >> 8);  \
  (dst)[2] = (src)[2] ^ U8(*(pad) >> 16); \
  (dst)[3] = (src)[3] ^ U8(*(pad) >> 24);
#endif

#define index8_32(a, ix) ((a) + ((ix) * sizeof(uint32_t)))

#define xor32_blocks(dest, source, pad, words)                    \
  for (i = 0; i < words; i++) {                                   \
    xor32_le(index8_32(dest, i), index8_32(source, i), (pad) + i) \
  }

static void xor_block(uint8_t *dest, const uint8_t *source,
                      const uint32_t *pad, unsigned int chunk_size) {
  unsigned int i, full_blocks = chunk_size / (unsigned int)sizeof(uint32_t);
  xor32_blocks(dest, source, pad, full_blocks)
  dest += full_blocks * sizeof(uint32_t);
  source += full_blocks * sizeof(uint32_t);
  pad += full_blocks;
  switch (chunk_size % sizeof(uint32_t)) {
    case 1: dest[0] = source[0] ^ U8(*pad); break;
    case 2: dest[0] = source[0] ^ U8(*pad);
            dest[1] = source[1] ^ U8(*pad >> 8); break;
    case 3: dest[0] = source[0] ^ U8(*pad);
            dest[1] = source[1] ^ U8(*pad >> 8);
            dest[2] = source[2] ^ U8(*pad >> 16); break;
  }
}

static void chacha20_xor_stream(uint8_t *dest, const uint8_t *source,
                                size_t length, const uint8_t key[CHACHA20_KEY_SIZE],
                                const uint8_t nonce[CHACHA20_NONCE_SIZE],
                                uint32_t counter) {
  uint32_t state[CHACHA20_STATE_WORDS];
  uint32_t pad[CHACHA20_STATE_WORDS];
  size_t i, b, last_block, full_blocks = length / CHACHA20_BLOCK_SIZE;
  initialize_state(state, key, nonce, counter);
  for (b = 0; b < full_blocks; b++) {
    core_block(state, pad);
    state[12]++;
    xor32_blocks(dest, source, pad, CHACHA20_STATE_WORDS)
    dest += CHACHA20_BLOCK_SIZE;
    source += CHACHA20_BLOCK_SIZE;
  }
  last_block = length % CHACHA20_BLOCK_SIZE;
  if (last_block > 0) {
    core_block(state, pad);
    xor_block(dest, source, pad, (unsigned int)last_block);
  }
}

#ifdef HAVE_LITTLE_ENDIAN
#define serialize(poly_key, result) memcpy(poly_key, result, 32)
#else
#define store32_le(target, source)   \
  (target)[0] = U8(*(source));       \
  (target)[1] = U8(*(source) >> 8);  \
  (target)[2] = U8(*(source) >> 16); \
  (target)[3] = U8(*(source) >> 24);
#define serialize(poly_key, result)                 \
  for (i = 0; i < 32 / sizeof(uint32_t); i++) {     \
    store32_le(index8_32(poly_key, i), result + i); \
  }
#endif

static void rfc8439_keygen(uint8_t poly_key[32],
                           const uint8_t key[CHACHA20_KEY_SIZE],
                           const uint8_t nonce[CHACHA20_NONCE_SIZE]) {
  uint32_t state[CHACHA20_STATE_WORDS];
  uint32_t result[CHACHA20_STATE_WORDS];
  size_t i;
  initialize_state(state, key, nonce, 0);
  core_block(state, result);
  serialize(poly_key, result);
  (void)i;
}

// ─────────────────────────────────────────────
//  Poly1305 MAC (32-bit path)
// ─────────────────────────────────────────────

#define poly1305_block_size 16

typedef struct {
  size_t aligner;
  unsigned char opaque[136];
} poly1305_context;

typedef struct {
  unsigned long r[5];
  unsigned long h[5];
  unsigned long pad[4];
  size_t leftover;
  unsigned char buffer[poly1305_block_size];
  unsigned char final;
} poly1305_state_internal_t;

static unsigned long U8TO32(const unsigned char *p) {
  return (((unsigned long)(p[0] & 0xff)) |
          ((unsigned long)(p[1] & 0xff) << 8) |
          ((unsigned long)(p[2] & 0xff) << 16) |
          ((unsigned long)(p[3] & 0xff) << 24));
}

static void U32TO8(unsigned char *p, unsigned long v) {
  p[0] = (unsigned char)((v) & 0xff);
  p[1] = (unsigned char)((v >> 8) & 0xff);
  p[2] = (unsigned char)((v >> 16) & 0xff);
  p[3] = (unsigned char)((v >> 24) & 0xff);
}

static void poly1305_init(poly1305_context *ctx, const unsigned char key[32]) {
  poly1305_state_internal_t *st = (poly1305_state_internal_t *)ctx;
  st->r[0] = (U8TO32(&key[0])) & 0x3ffffff;
  st->r[1] = (U8TO32(&key[3]) >> 2) & 0x3ffff03;
  st->r[2] = (U8TO32(&key[6]) >> 4) & 0x3ffc0ff;
  st->r[3] = (U8TO32(&key[9]) >> 6) & 0x3f03fff;
  st->r[4] = (U8TO32(&key[12]) >> 8) & 0x00fffff;
  st->h[0] = 0; st->h[1] = 0; st->h[2] = 0; st->h[3] = 0; st->h[4] = 0;
  st->pad[0] = U8TO32(&key[16]);
  st->pad[1] = U8TO32(&key[20]);
  st->pad[2] = U8TO32(&key[24]);
  st->pad[3] = U8TO32(&key[28]);
  st->leftover = 0;
  st->final = 0;
}

static void poly1305_blocks(poly1305_state_internal_t *st,
                            const unsigned char *m, size_t bytes) {
  const unsigned long hibit = (st->final) ? 0 : (1UL << 24);
  unsigned long r0, r1, r2, r3, r4;
  unsigned long s1, s2, s3, s4;
  unsigned long h0, h1, h2, h3, h4;
  uint64_t d0, d1, d2, d3, d4;
  unsigned long c;

  r0 = st->r[0]; r1 = st->r[1]; r2 = st->r[2]; r3 = st->r[3]; r4 = st->r[4];
  s1 = r1 * 5; s2 = r2 * 5; s3 = r3 * 5; s4 = r4 * 5;
  h0 = st->h[0]; h1 = st->h[1]; h2 = st->h[2]; h3 = st->h[3]; h4 = st->h[4];

  while (bytes >= poly1305_block_size) {
    h0 += (U8TO32(m + 0)) & 0x3ffffff;
    h1 += (U8TO32(m + 3) >> 2) & 0x3ffffff;
    h2 += (U8TO32(m + 6) >> 4) & 0x3ffffff;
    h3 += (U8TO32(m + 9) >> 6) & 0x3ffffff;
    h4 += (U8TO32(m + 12) >> 8) | hibit;

    d0 = ((uint64_t)h0 * r0) + ((uint64_t)h1 * s4) + ((uint64_t)h2 * s3) +
         ((uint64_t)h3 * s2) + ((uint64_t)h4 * s1);
    d1 = ((uint64_t)h0 * r1) + ((uint64_t)h1 * r0) + ((uint64_t)h2 * s4) +
         ((uint64_t)h3 * s3) + ((uint64_t)h4 * s2);
    d2 = ((uint64_t)h0 * r2) + ((uint64_t)h1 * r1) + ((uint64_t)h2 * r0) +
         ((uint64_t)h3 * s4) + ((uint64_t)h4 * s3);
    d3 = ((uint64_t)h0 * r3) + ((uint64_t)h1 * r2) + ((uint64_t)h2 * r1) +
         ((uint64_t)h3 * r0) + ((uint64_t)h4 * s4);
    d4 = ((uint64_t)h0 * r4) + ((uint64_t)h1 * r3) + ((uint64_t)h2 * r2) +
         ((uint64_t)h3 * r1) + ((uint64_t)h4 * r0);

    c = (unsigned long)(d0 >> 26); h0 = (unsigned long)d0 & 0x3ffffff; d1 += c;
    c = (unsigned long)(d1 >> 26); h1 = (unsigned long)d1 & 0x3ffffff; d2 += c;
    c = (unsigned long)(d2 >> 26); h2 = (unsigned long)d2 & 0x3ffffff; d3 += c;
    c = (unsigned long)(d3 >> 26); h3 = (unsigned long)d3 & 0x3ffffff; d4 += c;
    c = (unsigned long)(d4 >> 26); h4 = (unsigned long)d4 & 0x3ffffff;
    h0 += c * 5; c = (h0 >> 26); h0 = h0 & 0x3ffffff; h1 += c;

    m += poly1305_block_size;
    bytes -= poly1305_block_size;
  }

  st->h[0] = h0; st->h[1] = h1; st->h[2] = h2; st->h[3] = h3; st->h[4] = h4;
}

static void poly1305_finish(poly1305_context *ctx, unsigned char mac[16]) {
  poly1305_state_internal_t *st = (poly1305_state_internal_t *)ctx;
  unsigned long h0, h1, h2, h3, h4, c;
  unsigned long g0, g1, g2, g3, g4;
  uint64_t f;
  unsigned long mask;

  if (st->leftover) {
    size_t i = st->leftover;
    st->buffer[i++] = 1;
    for (; i < poly1305_block_size; i++) st->buffer[i] = 0;
    st->final = 1;
    poly1305_blocks(st, st->buffer, poly1305_block_size);
  }

  h0 = st->h[0]; h1 = st->h[1]; h2 = st->h[2]; h3 = st->h[3]; h4 = st->h[4];

  c = h1 >> 26; h1 &= 0x3ffffff; h2 += c;
  c = h2 >> 26; h2 &= 0x3ffffff; h3 += c;
  c = h3 >> 26; h3 &= 0x3ffffff; h4 += c;
  c = h4 >> 26; h4 &= 0x3ffffff; h0 += c * 5;
  c = h0 >> 26; h0 &= 0x3ffffff; h1 += c;

  g0 = h0 + 5; c = g0 >> 26; g0 &= 0x3ffffff;
  g1 = h1 + c;  c = g1 >> 26; g1 &= 0x3ffffff;
  g2 = h2 + c;  c = g2 >> 26; g2 &= 0x3ffffff;
  g3 = h3 + c;  c = g3 >> 26; g3 &= 0x3ffffff;
  g4 = h4 + c - (1UL << 26);

  mask = (g4 >> ((sizeof(unsigned long) * 8) - 1)) - 1;
  g0 &= mask; g1 &= mask; g2 &= mask; g3 &= mask; g4 &= mask;
  mask = ~mask;
  h0 = (h0 & mask) | g0; h1 = (h1 & mask) | g1;
  h2 = (h2 & mask) | g2; h3 = (h3 & mask) | g3;
  h4 = (h4 & mask) | g4;

  h0 = ((h0) | (h1 << 26)) & 0xffffffff;
  h1 = ((h1 >> 6) | (h2 << 20)) & 0xffffffff;
  h2 = ((h2 >> 12) | (h3 << 14)) & 0xffffffff;
  h3 = ((h3 >> 18) | (h4 << 8)) & 0xffffffff;

  f = (uint64_t)h0 + st->pad[0];             h0 = (unsigned long)f;
  f = (uint64_t)h1 + st->pad[1] + (f >> 32); h1 = (unsigned long)f;
  f = (uint64_t)h2 + st->pad[2] + (f >> 32); h2 = (unsigned long)f;
  f = (uint64_t)h3 + st->pad[3] + (f >> 32); h3 = (unsigned long)f;

  U32TO8(mac + 0, h0); U32TO8(mac + 4, h1);
  U32TO8(mac + 8, h2); U32TO8(mac + 12, h3);

  st->h[0] = 0; st->h[1] = 0; st->h[2] = 0; st->h[3] = 0; st->h[4] = 0;
  st->r[0] = 0; st->r[1] = 0; st->r[2] = 0; st->r[3] = 0; st->r[4] = 0;
  st->pad[0] = 0; st->pad[1] = 0; st->pad[2] = 0; st->pad[3] = 0;
}

static void poly1305_update(poly1305_context *ctx, const unsigned char *m,
                            size_t bytes) {
  poly1305_state_internal_t *st = (poly1305_state_internal_t *)ctx;
  size_t i;

  if (st->leftover) {
    size_t want = (poly1305_block_size - st->leftover);
    if (want > bytes) want = bytes;
    for (i = 0; i < want; i++) st->buffer[st->leftover + i] = m[i];
    bytes -= want;
    m += want;
    st->leftover += want;
    if (st->leftover < poly1305_block_size) return;
    poly1305_blocks(st, st->buffer, poly1305_block_size);
    st->leftover = 0;
  }

  if (bytes >= poly1305_block_size) {
    size_t want = (bytes & (size_t)~(poly1305_block_size - 1));
    poly1305_blocks(st, m, want);
    m += want;
    bytes -= want;
  }

  if (bytes) {
    for (i = 0; i < bytes; i++) st->buffer[st->leftover + i] = m[i];
    st->leftover += bytes;
  }
}

// ─────────────────────────────────────────────
//  RFC 8439 AEAD
// ─────────────────────────────────────────────

static uint8_t ZEROES[16] = {0};

static void pad_if_needed(poly1305_context *ctx, size_t size) {
  size_t padding = size % 16;
  if (padding != 0) poly1305_update(ctx, ZEROES, 16 - padding);
}

static void write_64bit_int(poly1305_context *ctx, uint64_t value) {
  uint8_t result[8];
  result[0] = (uint8_t)(value);
  result[1] = (uint8_t)(value >> 8);
  result[2] = (uint8_t)(value >> 16);
  result[3] = (uint8_t)(value >> 24);
  result[4] = (uint8_t)(value >> 32);
  result[5] = (uint8_t)(value >> 40);
  result[6] = (uint8_t)(value >> 48);
  result[7] = (uint8_t)(value >> 56);
  poly1305_update(ctx, result, 8);
}

static void poly1305_calculate_mac(
    uint8_t *mac, const uint8_t *cipher_text, size_t cipher_text_size,
    const uint8_t key[32], const uint8_t nonce[12],
    const uint8_t *ad, size_t ad_size) {
  uint8_t poly_key[32] = {0};
  poly1305_context poly_ctx;
  rfc8439_keygen(poly_key, key, nonce);
  poly1305_init(&poly_ctx, poly_key);
  if (ad != NULL && ad_size > 0) {
    poly1305_update(&poly_ctx, ad, ad_size);
    pad_if_needed(&poly_ctx, ad_size);
  }
  poly1305_update(&poly_ctx, cipher_text, cipher_text_size);
  pad_if_needed(&poly_ctx, cipher_text_size);
  write_64bit_int(&poly_ctx, ad_size);
  write_64bit_int(&poly_ctx, cipher_text_size);
  poly1305_finish(&poly_ctx, mac);
}

#define PM(p) ((size_t)(p))
#define OVERLAPPING(s, s_size, b, b_size) \
  (PM(s) < PM((b) + (b_size))) && (PM(b) < PM((s) + (s_size)))

size_t chacha20_poly1305_encrypt(
    uint8_t *cipher_text, const uint8_t key[32],
    const uint8_t nonce[12], const uint8_t *ad, size_t ad_size,
    const uint8_t *plain_text, size_t plain_text_size) {
  size_t new_size = plain_text_size + RFC_8439_TAG_SIZE;
  if (OVERLAPPING(plain_text, plain_text_size, cipher_text, new_size))
    return (size_t)-1;
  chacha20_xor_stream(cipher_text, plain_text, plain_text_size, key, nonce, 1);
  poly1305_calculate_mac(cipher_text + plain_text_size, cipher_text,
                         plain_text_size, key, nonce, ad, ad_size);
  return new_size;
}

size_t chacha20_poly1305_decrypt(
    uint8_t *plain_text, const uint8_t key[32],
    const uint8_t nonce[12],
    const uint8_t *cipher_text, size_t cipher_text_size) {
  size_t actual_size = cipher_text_size - RFC_8439_TAG_SIZE;
  if (OVERLAPPING(plain_text, actual_size, cipher_text, cipher_text_size))
    return (size_t)-1;
  chacha20_xor_stream(plain_text, cipher_text, actual_size, key, nonce, 1);
  return actual_size;
}
