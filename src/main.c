#include "fabada256.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s STRING\n", argv[0]);
        return 1;
    }
    uint8_t digest[FABADA256_DIGEST_SIZE];
    fabada256(digest, (const uint8_t *)argv[1], strlen(argv[1]));
    for (size_t i = 0; i < sizeof(digest); ++i) {
        printf("%02x", digest[i]);
    }
    putchar('\n');
    return 0;
}
