#include  <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include  "stack.h"
#include   "hash.h"

static uint32_t murmur_32_scramble(uint32_t k);

//--------------------------------- MUR MUR 32 HASH --------------------------------

uint32_t murmurhash_32(const uint8_t* key, size_t len, uint32_t seed) {
    assert(key != NULL);

    uint32_t k = 0;
	uint32_t h = seed;

    for (size_t i = len >> 2; i; i--) {
        memcpy(&k, key, sizeof(uint32_t));
        key += sizeof(uint32_t);
        h ^= murmur_32_scramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }

    k = 0;
    for (size_t i = len & 3; i; i--) {
        k <<= 8;
        k |= key[i - 1];
    }

    h ^= murmur_32_scramble(k);

	h ^= len;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}

//--------------------------------- MUR MUR 32 SCRUMBLE ---------------------------

static uint32_t murmur_32_scramble(uint32_t k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}













































































































































































