#include <TestU01.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fabada256.h"

typedef enum {
    MODE_TEST,
    MODE_SMALLCRUSH,
    MODE_CRUSH,
    MODE_BIGCRUSH
} test_mode_t;

typedef struct {
    uint32_t seed;
    uint32_t counter;
    uint8_t digest[32];
    size_t next;
} fabada256_rng_state_t;

static fabada256_rng_state_t rng_state;

static uint32_t load32le(const uint8_t *p)
{
    return ((uint32_t)p[0])       |
           ((uint32_t)p[1] << 8)  |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void store32le(uint8_t *p, uint32_t x)
{
    p[0] = (uint8_t)x;
    p[1] = (uint8_t)(x >> 8);
    p[2] = (uint8_t)(x >> 16);
    p[3] = (uint8_t)(x >> 24);
}

static void fabada256_rng_seed(uint32_t seed)
{
    rng_state.seed = seed;
    rng_state.counter = 0;
    rng_state.next = sizeof(rng_state.digest); /* force initial refill */
}

static unsigned int fabada256_rng32(void)
{
    if (rng_state.next == sizeof(rng_state.digest)) {
        uint8_t input[8];

        store32le(input, rng_state.seed);
        store32le(input + 4, rng_state.counter++);

        fabada256(rng_state.digest, input, sizeof(input));
        rng_state.next = 0;
    }

    uint32_t result = load32le(rng_state.digest + rng_state.next);
    rng_state.next += sizeof(result);
    return result;
}

int main(int argc, char** argv) {
    test_mode_t mode = MODE_SMALLCRUSH;
    uint32_t seed = 0xbabababa;

     for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "--test") == 0) {
            mode = MODE_TEST;
        } else if (strcmp(arg, "--smallcrush") == 0) {
            mode = MODE_SMALLCRUSH;
        } else if (strcmp(arg, "--crush") == 0) {
            mode = MODE_CRUSH;
        } else if (strcmp(arg, "--bigcrush") == 0) {
            mode = MODE_BIGCRUSH;
        } else if (strncmp(arg, "--seed=", 7) == 0) {
            const char *value = arg + 7;
            char *end = NULL;
            errno = 0;
            unsigned long long parsed = strtoull(value, &end, 0);

            if (end == value ||
                *end != '\0' ||
                errno == ERANGE ||
                parsed > UINT32_MAX) {
                fprintf(stderr, "Invalid seed: %s\n", value);
                return EXIT_FAILURE;
            }

            seed = (uint32_t)parsed;
        }
    }

    fabada256_rng_seed(seed);

    unif01_Gen *gen = unif01_CreateExternGenBits("fabada256", fabada256_rng32);

    switch (mode) {
    case MODE_TEST:
        uint32_t r = fabada256_rng32();
        printf("%04x\n", r);
        break;
    case MODE_SMALLCRUSH:
        printf("Running SmallCrush...\n");
        bbattery_SmallCrush(gen);
        break;
    case MODE_CRUSH:
        printf("Running Crush...\n");
        bbattery_Crush(gen);
        break;
    case MODE_BIGCRUSH:
        printf("Running BigCrush...\n");
        bbattery_BigCrush(gen);
        break;
    }

    unif01_DeleteExternGenBits(gen);
}
