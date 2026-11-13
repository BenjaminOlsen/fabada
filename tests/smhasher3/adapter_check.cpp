/* Check the registered adapter, not a duplicate of its wrapper. */
#include "Platform.h"
#include "Hashlib.h"
#include "fabada256.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    USE_FAMILY(FABADA);
    const HashInfo *h = findHash("Fabada256");
    assert(h && h->bits == 256 && (h->hash_flags & FLAG_HASH_NO_SEED));
    uint8_t input[1025], expected[32], actual[32];
    for (size_t i = 0; i < sizeof(input); ++i) input[i] = static_cast<uint8_t>(73 * i);
    const HashInfo::endianness modes[] = {HashInfo::ENDIAN_NATIVE, HashInfo::ENDIAN_BYTESWAPPED};
    const seed_t seeds[] = {0, 1, UINT64_C(0xffffffffffffffff)};
    for (auto mode : modes) {
        assert(h->ComputedVerify(mode) == 0x96D25973);
        for (auto seed : seeds) {
            for (size_t n = 0; n <= 1024; ++n) {
                fabada256(expected, input + 1, n); // deliberately unaligned
                h->hashFn(mode)(input + 1, n, seed, actual);
                assert(std::memcmp(expected, actual, 32) == 0);
            }
        }
    }
    std::puts("Fabada256: registered 256-bit unseeded adapter; both modes match; verification 96D25973");
}
