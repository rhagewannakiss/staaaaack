#ifndef HASH_H_
#define HASH_H_

#include  <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include  "stack.h"

uint32_t murmurhash_32(const uint8_t* key, size_t len, uint32_t seed);

#endif //  HASH_H_