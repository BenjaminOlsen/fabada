#ifndef FABADA256_H
#define FABADA256_H

#include <stdint.h>
#include <stddef.h>

#define FABADA256_BLOCK_SIZE 32
#define FABADA256_DIGEST_SIZE 32

#ifdef __cplusplus
extern "C" {
#endif

/* Call fabada256_init before use; fabada256_final is terminal:
   reinitialize the context before hashing another message. */
typedef struct {
    uint32_t state[8];
    uint64_t total_bits;   /* total message length in bits (mod 2^64) */
    uint8_t buffer[FABADA256_BLOCK_SIZE];    /* partial block buffer */
    size_t buffer_len;     /* number of bytes currently in buffer [0..FABADA256_BLOCK_SIZE) */
} fabada256_state_t;

void fabada256_init(fabada256_state_t *st);
void fabada256_update(fabada256_state_t *st, const uint8_t *data, size_t len);
void fabada256_final(fabada256_state_t *st, uint8_t out[32]);
void fabada256(uint8_t out[32], const uint8_t* data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* FABADA256_H */
