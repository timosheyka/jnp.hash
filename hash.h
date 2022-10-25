#ifndef JNP1_HASH_H
#define JNP1_HASH_H

#include <stddef.h>

#ifdef __cplusplus

#include <cstdint>

namespace jnp1 {
	
#else

#include <stdint.h>
#include <stdbool.h>

extern "C" {

#endif

// unsigned long hash_create(hash_function_t hash_function);
unsigned long hash_create(uint64_t (*hash_function)(uint64_t const *, size_t));

void hash_delete(unsigned long id);

size_t hash_size(unsigned long id);

bool hash_insert(unsigned long id, uint64_t const * seq, size_t size);

bool hash_remove(unsigned long id, uint64_t const * seq, size_t size);

void hash_clear(unsigned long id);

bool hash_test(unsigned long id, uint64_t const * seq, size_t size);

}

#endif // JNP1_HASH_H
