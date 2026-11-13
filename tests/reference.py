#!/usr/bin/env python3
"""Independent model of the current hash. Print vectors and SMHasher3's unseeded verification code.

Uses only Python's standard library; never imports or calls the C implementation.
Message pattern for boundary vectors: byte i = (73*i) mod 256.
"""
import struct

MASK = 0xffffffff
CONSTANTS = (0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344, 0xa4093822,
             0x299f31d0, 0x082efa98, 0xec4e6c89, 0x452821e6, 0x38d01377)


def rol(x, n):
    return ((x << n) | (x >> (32 - n))) & MASK


def digest(message):
    state = [rol(0xfabadaaa, (32 - r) % 32) for r in (0, 3, 5, 7, 11, 13, 17, 23)]
    bits = (len(message) * 8) % (1 << 64)
    padded = message + b'\x80'
    padded += b'\0' * ((24 - len(padded)) % 32) + struct.pack('>Q', bits)
    for offset in range(0, len(padded), 32):
        w = state + list(struct.unpack('<8I', padded[offset:offset + 32]))
        final = offset + 32 == len(padded)
        for constant in CONSTANTS:
            w[0] ^= constant
            w[15] ^= MASK if final else 0
            for a, b, c, d in ((0, 4, 8, 12), (1, 5, 9, 13), (2, 6, 10, 14),
                               (3, 7, 11, 15), (0, 5, 10, 15), (1, 6, 11, 12),
                               (2, 7, 8, 13), (3, 4, 9, 14)):
                w[a] = (w[a] + w[b]) & MASK
                w[d] = rol(w[d] ^ w[a], 16)
                w[c] = (w[c] + w[d]) & MASK
                w[b] = rol(w[b] ^ w[c], 12)
                w[a] = (w[a] + w[b]) & MASK
                w[d] = rol(w[d] ^ w[a], 8)
                w[c] = (w[c] + w[d]) & MASK
                w[b] = rol(w[b] ^ w[c], 7)
        state = [a ^ b for a, b in zip(state, w)]
    return struct.pack('<8I', *state)


def verification():
    hashes = b''.join(digest(bytes(range(n))) for n in range(256))
    return int.from_bytes(digest(hashes)[:4], 'little')


if __name__ == '__main__':
    for n in (0, 1, 23, 24, 31, 32, 33, 55, 56, 63, 64, 65, 193, 1024):
        print(n, digest(bytes((73 * i) % 256 for i in range(n))).hex())
    print('abc', digest(b'abc').hex())
    print(f'SMHasher3 LE and BE: 0x{verification():08X}')
