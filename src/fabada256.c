#include "fabada256.h"
#include <stdint.h>
#include <string.h>

#include "utils.h"

void fabada256_init(fabada256_state_t *st) {
    st->state[0] = rotr32(0xfabadaaau, 0);
    st->state[1] = rotr32(0xfabadaaau, 3);
    st->state[2] = rotr32(0xfabadaaau, 5);
    st->state[3] = rotr32(0xfabadaaau, 7);
    st->state[4] = rotr32(0xfabadaaau, 11);
    st->state[5] = rotr32(0xfabadaaau, 13);
    st->state[6] = rotr32(0xfabadaaau, 17);
    st->state[7] = rotr32(0xfabadaaau, 23);

    st->total_bits = 0;
    st->buffer_len = 0;
}


#define QR(a, b, c, d) \
        a += b; d ^= a; d = rotl32(d, 16); \
        c += d; b ^= c; b = rotl32(b, 12); \
        a += b; d ^= a; d = rotl32(d,  8); \
        c += d; b ^= c; b = rotl32(b,  7);


/* public constants (hexadecimal digits of pi); not secret key material.
   Injected inside every double round to break the repeated-row invariant.
*/
static const uint32_t round_constants[10] = {
    0x243f6a88u, 0x85a308d3u, 0x13198a2eu, 0x03707344u, 0xa4093822u,
    0x299f31d0u, 0x082efa98u, 0xec4e6c89u, 0x452821e6u, 0x38d01377u
};

static void fabada256_compress_domain(fabada256_state_t *st,
                               const uint8_t block[FABADA256_BLOCK_SIZE], int final) {
    uint32_t w[16];
    for (int i = 0; i < 8; ++i) {
        w[i] = st->state[i];
        w[i+8] = load32le(block + 4*i);
    }
    
    for (int i = 0; i < 10; ++i) {
        w[0] ^= round_constants[i];
        /* Mix the final-block domain within EVERY double round. Applying
           a mask only to the input block could be canceled by message bytes;
           applying it only to the output would be publicly reversible. */
        w[15] ^= final ? UINT32_MAX : 0u;
        QR(w[0], w[4], w[ 8], w[12])
        QR(w[1], w[5], w[ 9], w[13])
        QR(w[2], w[6], w[10], w[14])
        QR(w[3], w[7], w[11], w[15])
        QR(w[0], w[5], w[10], w[15])
        QR(w[1], w[6], w[11], w[12])
        QR(w[2], w[7], w[ 8], w[13])
        QR(w[3], w[4], w[ 9], w[14])
    }

    
    for (int i = 0; i < 8; ++i) { /* Truncated permutation with XOR feedforward. */
        st->state[i] ^= w[i];
    }
}

void fabada256_compress(fabada256_state_t *st, const uint8_t block[FABADA256_BLOCK_SIZE]) {
    fabada256_compress_domain(st, block, 0);
}

void fabada256_update(fabada256_state_t *st, const uint8_t *data, size_t len) {

    if (len == 0) {
        return;
    }

    st->total_bits += (uint64_t)len * 8u;

    size_t buffer_len = st->buffer_len;

    if (buffer_len > 0) {
        size_t to_fill = FABADA256_BLOCK_SIZE - buffer_len;
        if (len < to_fill) {
            memcpy(st->buffer + buffer_len, data, len);
            st->buffer_len = buffer_len + len;
            return;
        }

        memcpy(st->buffer + buffer_len, data, to_fill);
        fabada256_compress(st, st->buffer);

        data        += to_fill;
        len         -= to_fill;
        buffer_len   = 0;
    }

    while (len >= FABADA256_BLOCK_SIZE) {
        fabada256_compress(st, data);
        data += FABADA256_BLOCK_SIZE;
        len  -= FABADA256_BLOCK_SIZE;
    }

    if (len > 0) {
        memcpy(st->buffer + buffer_len, data, len);
        st->buffer_len = buffer_len + len;
    } else {
        st->buffer_len = 0;
    }
}

void fabada256_final(fabada256_state_t *st, uint8_t out[32]) {
    uint8_t *buf = st->buffer;
    size_t i = st->buffer_len;
    buf[i++] = 0x80;

    /* if there isn't room for the total bit length (8 bytes) in this block,
        pad with zeros, compress, and start a new block: */
    if (i > FABADA256_BLOCK_SIZE - 8) {
        while (i < FABADA256_BLOCK_SIZE) {
            buf[i++] = 0;
        }
        fabada256_compress(st, buf);
        i = 0;
    }
    while (i < FABADA256_BLOCK_SIZE - 8) {
        buf[i++] = 0;
    }

    uint64_t bits = st->total_bits;
    buf[FABADA256_BLOCK_SIZE-8] = (uint8_t)(bits >> 56);
    buf[FABADA256_BLOCK_SIZE-7] = (uint8_t)(bits >> 48);
    buf[FABADA256_BLOCK_SIZE-6] = (uint8_t)(bits >> 40);
    buf[FABADA256_BLOCK_SIZE-5] = (uint8_t)(bits >> 32);
    buf[FABADA256_BLOCK_SIZE-4] = (uint8_t)(bits >> 24);
    buf[FABADA256_BLOCK_SIZE-3] = (uint8_t)(bits >> 16);
    buf[FABADA256_BLOCK_SIZE-2] = (uint8_t)(bits >> 8);
    buf[FABADA256_BLOCK_SIZE-1] = (uint8_t)(bits);

    /* only the last padding block belongs to the final-block domain. */
    fabada256_compress_domain(st, buf, 1);
    for (unsigned int j = 0; j < 8; ++j) {
        store32le(out + 4*j, st->state[j]);
    }
}
 
void fabada256(uint8_t out[32], const uint8_t *data, size_t len) {
    fabada256_state_t st;
    fabada256_init(&st);
    fabada256_update(&st, data, len);
    fabada256_final(&st, out);
}

