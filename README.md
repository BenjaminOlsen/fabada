# Fabada256

Experimental 256-bit hash, with basic unit tests, test vectors, and statistical tests.

Named after [fabada asturiana](https://web.archive.org/web/20261103064106/https://en.wikipedia.org/wiki/Fabada_asturiana), one of the greatest beany stews.

## Build and test

Requires CMake 3.20+ and a C11 compiler.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Hash a string with the command-line tool:

```sh
./build/fabada256 "abc"
# 81d68b80a55235157f0941058c516ace45a898f15ab2c01a33981012a6f52046
```

Use `-DBUILD_TESTING=OFF` to skip unit tests. 

```c
#include <fabada256.h>

uint8_t digest[32];
fabada256(digest, (const uint8_t *)"abc", 3);
```

Streaming uses `fabada256_init`, `fabada256_update`, and `fabada256_final`.
Reinitialize after finalization. Tests cover fixed vectors, padding boundaries,
and incremental updates. `tests/reference.py` independently generates vectors.

## TestU01

```sh
cmake -S . -B build-testu01 -DFABADA_BUILD_TESTU01=ON \
  -DTESTU01_ROOT=/your/TestU01/install
cmake --build build-testu01 --parallel
./build-testu01/test_testu01 --test
./build-testu01/test_testu01 --smallcrush --seed=0xbabababa
./build-testu01/test_testu01 --crush --seed=0xbabababa
./build-testu01/test_testu01 --bigcrush --seed=0xbabababa
```

Default: SmallCrush with seed `0xbabababa`. `--test` prints `867dd601` for that seed.

## SMHasher3

```sh
git submodule update --init --recursive
cmake -S . -B build-smhasher3 -DFABADA_BUILD_SMHASHER3=ON
cmake --build build-smhasher3 --target smhasher3 -j
cmake --build build-smhasher3 --target smhasher3-adapter-check
cmake --build build-smhasher3 --target smhasher3-quick
cmake --build build-smhasher3 --target smhasher3-quality
```

The pinned upstream runs from a patched build-directory copy; the submodule stays
clean. 

'quick' runs Avalanche and BIC; 'quality' adds Bitflip, Sparse, TwoBytes,
Cyclic, Permutation, Zeroes and Text.
 
Builds never launch statistical batteries automatically.

