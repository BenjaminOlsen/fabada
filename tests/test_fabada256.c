/* Unit tests for the current Fabada256 API and fixed digest encoding. */
#include "fabada256.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void check_vectors(void) {
    /* Cross-checked against an independent Python implementation. */
    const char *messages[] = {"", "abc"};
    const char *expected[] = {
        "236a8cf323afaa7aaa8b91e49fbe14c3ada8c051c1284ba1c23defbabfdc1fcb",
        "81d68b80a55235157f0941058c516ace45a898f15ab2c01a33981012a6f52046"
    };
    const char hex[] = "0123456789abcdef";
    for (unsigned n = 0; n < 2; ++n) {
        uint8_t digest[32];
        char actual[65];
        fabada256(digest, (const uint8_t *)messages[n], strlen(messages[n]));
        for (unsigned i = 0; i < 32; ++i) {
            actual[2 * i] = hex[digest[i] >> 4];
            actual[2 * i + 1] = hex[digest[i] & 15];
        }
        actual[64] = '\0';
        assert(strcmp(actual, expected[n]) == 0);
    }
    puts("Independent reference vectors: empty message and abc matched");
}

#include "vectors.h"

static void check_boundaries(void) {
    uint8_t message[1024], digest[32];
    const char hex[] = "0123456789abcdef";
    for (size_t i = 0; i < sizeof(message); ++i) message[i] = (uint8_t)(73 * i);
    for (size_t n = 0; n < sizeof(boundary_vectors) / sizeof(boundary_vectors[0]); ++n) {
        fabada256(digest, message, boundary_vectors[n].length);
        for (size_t i = 0; i < sizeof(digest); ++i) {
            assert(hex[digest[i] >> 4] == boundary_vectors[n].hex[2 * i]);
            assert(hex[digest[i] & 15] == boundary_vectors[n].hex[2 * i + 1]);
        }
    }
    puts("14 independent reference vectors at padding and block boundaries matched");
}

static void check_streaming(void) {
    uint8_t message[193], expected[32], actual[32];
    for (size_t i = 0; i < sizeof(message); ++i) message[i] = (uint8_t)(i * 73);
    for (size_t len = 0; len <= sizeof(message); ++len) {
        fabada256(expected, message, len);
        for (size_t split = 0; split <= len; ++split) {
            fabada256_state_t s;
            fabada256_init(&s);
            fabada256_update(&s, message, split);
            fabada256_update(&s, NULL, 0);
            fabada256_update(&s, message + split, len - split);
            fabada256_final(&s, actual);
            assert(memcmp(expected, actual, 32) == 0);
        }
        fabada256_state_t s;
        fabada256_init(&s);
        for (size_t i = 0; i < len; ++i) fabada256_update(&s, message + i, 1);
        fabada256_final(&s, actual);
        assert(memcmp(expected, actual, 32) == 0);
    }
    puts("Streaming: all splits and bytewise updates, lengths 0 through 193");
}

int main(void) {
    check_vectors();
    check_boundaries();
    check_streaming();
    return 0;
}
