// Standalone X25519 Diffie-Hellman implementation
// Extracted from Mongoose library (Public Domain)
// Original source: https://github.com/cesanta/mongoose
//
// Pure math — no external dependencies beyond <stdint.h> and <string.h>

#include "x25519.h"
#include <string.h>

const uint8_t X25519_BASE_POINT[X25519_BYTES] = {9};

#define X25519_WBITS 32

typedef uint32_t limb_t;
typedef uint64_t dlimb_t;
typedef int64_t  sdlimb_t;

#define NLIMBS (256 / X25519_WBITS)
typedef limb_t fe[NLIMBS];

#define U32(a, b, c, d) \
  ((uint32_t) ((a) << 24 | (uint32_t) (b) << 16 | (c) << 8 | (d)))

static limb_t umaal(limb_t *carry, limb_t acc, limb_t mand, limb_t mier) {
  dlimb_t tmp = (dlimb_t) mand * mier + acc + *carry;
  *carry = (limb_t) (tmp >> X25519_WBITS);
  return (limb_t) tmp;
}

static limb_t adc(limb_t *carry, limb_t acc, limb_t mand) {
  dlimb_t total = (dlimb_t) *carry + acc + mand;
  *carry = (limb_t) (total >> X25519_WBITS);
  return (limb_t) total;
}

static limb_t adc0(limb_t *carry, limb_t acc) {
  dlimb_t total = (dlimb_t) *carry + acc;
  *carry = (limb_t) (total >> X25519_WBITS);
  return (limb_t) total;
}

static void propagate(fe x, limb_t over) {
  unsigned i;
  limb_t carry;
  over = x[NLIMBS - 1] >> (X25519_WBITS - 1) | over << 1;
  x[NLIMBS - 1] &= ~((limb_t) 1 << (X25519_WBITS - 1));
  carry = over * 19;
  for (i = 0; i < NLIMBS; i++) {
    x[i] = adc0(&carry, x[i]);
  }
}

static void fe_add(fe out, const fe a, const fe b) {
  unsigned i;
  limb_t carry = 0;
  for (i = 0; i < NLIMBS; i++) {
    out[i] = adc(&carry, a[i], b[i]);
  }
  propagate(out, carry);
}

static void fe_sub(fe out, const fe a, const fe b) {
  unsigned i;
  sdlimb_t carry = -38;
  for (i = 0; i < NLIMBS; i++) {
    carry = carry + a[i] - b[i];
    out[i] = (limb_t) carry;
    carry >>= X25519_WBITS;
  }
  propagate(out, (limb_t) (1 + carry));
}

static void fe_mul(fe out, const fe a, const limb_t *b, unsigned nb) {
  limb_t accum[2 * NLIMBS] = {0};
  unsigned i, j;
  limb_t carry2;
  for (i = 0; i < nb; i++) {
    limb_t mand = b[i];
    carry2 = 0;
    for (j = 0; j < NLIMBS; j++) {
      limb_t tmp;
      memcpy(&tmp, &a[j], sizeof(tmp));
      accum[i + j] = umaal(&carry2, accum[i + j], mand, tmp);
    }
    accum[i + j] = carry2;
  }
  carry2 = 0;
  for (j = 0; j < NLIMBS; j++) {
    out[j] = umaal(&carry2, accum[j], 38, accum[j + NLIMBS]);
  }
  propagate(out, carry2);
}

static void fe_sqr(fe out, const fe a) {
  fe_mul(out, a, a, NLIMBS);
}

static void fe_mul1(fe out, const fe a) {
  fe_mul(out, a, out, NLIMBS);
}

static void fe_sqr1(fe a) {
  fe_mul1(a, a);
}

static void condswap(limb_t a[2 * NLIMBS], limb_t b[2 * NLIMBS],
                     limb_t doswap) {
  unsigned i;
  for (i = 0; i < 2 * NLIMBS; i++) {
    limb_t xor_ab = (a[i] ^ b[i]) & doswap;
    a[i] ^= xor_ab;
    b[i] ^= xor_ab;
  }
}

static limb_t canon(fe x) {
  unsigned i;
  limb_t carry0 = 19;
  limb_t res;
  sdlimb_t carry;
  for (i = 0; i < NLIMBS; i++) {
    x[i] = adc0(&carry0, x[i]);
  }
  propagate(x, carry0);
  carry = -19;
  res = 0;
  for (i = 0; i < NLIMBS; i++) {
    carry += x[i];
    res |= x[i] = (limb_t) carry;
    carry >>= X25519_WBITS;
  }
  return (limb_t) (((dlimb_t) res - 1) >> X25519_WBITS);
}

static const limb_t a24[1] = {121665};

static void ladder_part1(fe xs[5]) {
  limb_t *x2 = xs[0], *z2 = xs[1], *x3 = xs[2], *z3 = xs[3], *t1 = xs[4];
  fe_add(t1, x2, z2);
  fe_sub(z2, x2, z2);
  fe_add(x2, x3, z3);
  fe_sub(z3, x3, z3);
  fe_mul1(z3, t1);
  fe_mul1(x2, z2);
  fe_add(x3, z3, x2);
  fe_sub(z3, z3, x2);
  fe_sqr1(t1);
  fe_sqr1(z2);
  fe_sub(x2, t1, z2);
  fe_mul(z2, x2, a24, sizeof(a24) / sizeof(a24[0]));
  fe_add(z2, z2, t1);
}

static void ladder_part2(fe xs[5], const fe x1) {
  limb_t *x2 = xs[0], *z2 = xs[1], *x3 = xs[2], *z3 = xs[3], *t1 = xs[4];
  fe_sqr1(z3);
  fe_mul1(z3, x1);
  fe_sqr1(x3);
  fe_mul1(z2, x2);
  fe_sub(x2, t1, x2);
  fe_mul1(x2, t1);
}

static void x25519_core(fe xs[5], const uint8_t scalar[X25519_BYTES],
                        const uint8_t *x1, int clamp) {
  int i;
  fe x1_limbs;
  limb_t swap = 0;
  limb_t *x2 = xs[0], *x3 = xs[2], *z3 = xs[3];
  memset(xs, 0, 4 * sizeof(fe));
  x2[0] = z3[0] = 1;
  for (i = 0; i < NLIMBS; i++) {
    x3[i] = x1_limbs[i] =
        U32(x1[i * 4 + 3], x1[i * 4 + 2], x1[i * 4 + 1], x1[i * 4]);
  }
  for (i = 255; i >= 0; i--) {
    uint8_t bytei = scalar[i / 8];
    limb_t doswap;
    if (clamp) {
      if (i / 8 == 0) {
        bytei &= (uint8_t) ~7U;
      } else if (i / 8 == X25519_BYTES - 1) {
        bytei &= 0x7F;
        bytei |= 0x40;
      }
    }
    doswap = 0 - (limb_t) ((bytei >> (i % 8)) & 1);
    condswap(x2, x3, swap ^ doswap);
    swap = doswap;
    ladder_part1(xs);
    ladder_part2(xs, (const limb_t *) x1_limbs);
  }
  condswap(x2, x3, swap);
}

int x25519(uint8_t out[X25519_BYTES], const uint8_t scalar[X25519_BYTES],
           const uint8_t x1[X25519_BYTES], int clamp) {
  int i, ret;
  fe xs[5], out_limbs;
  limb_t *x2, *z2, *z3, *prev;
  static const struct { uint8_t a, c, n; } steps[13] = {
      {2, 1, 1},  {2, 1, 1},  {4, 2, 3},  {2, 4, 6},  {3, 1, 1},
      {3, 2, 12}, {4, 3, 25}, {2, 3, 25}, {2, 4, 50}, {3, 2, 125},
      {3, 1, 2},  {3, 1, 2},  {3, 1, 1}};
  x25519_core(xs, scalar, x1, clamp);
  x2 = xs[0];
  z2 = xs[1];
  z3 = xs[3];
  prev = z2;
  for (i = 0; i < 13; i++) {
    int j;
    limb_t *a = xs[steps[i].a];
    for (j = steps[i].n; j > 0; j--) {
      fe_sqr(a, prev);
      prev = a;
    }
    fe_mul1(a, xs[steps[i].c]);
  }
  fe_mul(out_limbs, x2, z3, NLIMBS);
  ret = (int) canon(out_limbs);
  if (!clamp) ret = 0;
  for (i = 0; i < NLIMBS; i++) {
    uint32_t n = out_limbs[i];
    out[i * 4] = (uint8_t) (n & 0xff);
    out[i * 4 + 1] = (uint8_t) ((n >> 8) & 0xff);
    out[i * 4 + 2] = (uint8_t) ((n >> 16) & 0xff);
    out[i * 4 + 3] = (uint8_t) ((n >> 24) & 0xff);
  }
  return ret;
}
