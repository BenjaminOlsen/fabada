#ifndef FABADA256_PRIVATE_UTILS_H
#define FABADA256_PRIVATE_UTILS_H

#include <stdint.h>

static inline uint32_t rotl32(uint32_t x, unsigned r) {
    r &= 31u;
    return (x << r) | (x >> ((32u - r) & 31u));
}

static inline uint32_t rotr32(uint32_t x, unsigned r) {
    r &= 31u;
    return (x >> r) | (x << ((32u - r) & 31u));
}

static inline uint32_t load32le(const void* src) {
    const uint8_t *p = (const uint8_t*)src;
    uint32_t x = p[0];
    x |= (uint32_t)p[1] << 8;
    x |= (uint32_t)p[2] << 16;
    x |= (uint32_t)p[3] << 24;
    return x;
}

static inline void store32le(void *dst, uint32_t x) {
    uint8_t *p = (uint8_t*)dst;
    p[0] = (uint8_t)x & 0xFF;
    p[1] = (x >> 8);
    p[2] = (x >> 16);
    p[3] = (x >> 24);
}

#endif

