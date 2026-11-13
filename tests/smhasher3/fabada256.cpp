/* Fabada-owned SMHasher3 adapter. Output is always the library's 32 digest bytes. */
#include "Platform.h"
#include "Hashlib.h"
#include "fabada256.h"

static void Fabada256(const void *input, size_t length, seed_t seed, void *output) {
    (void)seed; // This hash has no seed. Do not create an artificial seeded variant.
    fabada256(static_cast<uint8_t *>(output), static_cast<const uint8_t *>(input), length);
}

REGISTER_FAMILY(FABADA,
    $.src_status = HashFamilyInfo::SRC_ACTIVE
);
REGISTER_HASH(Fabada256,
    $.desc = "Experimental Fabada256 (full unseeded digest)",
    $.hash_flags = FLAG_HASH_NO_SEED | FLAG_HASH_ENDIAN_INDEPENDENT,
    $.impl_flags = FLAG_IMPL_CANONICAL_BOTH,
    $.bits = 256,
    $.verification_LE = 0x96D25973,
    $.verification_BE = 0x96D25973,
    $.hashfn_native = Fabada256,
    $.hashfn_bswap = Fabada256
);
