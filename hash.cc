#include "hash.h"

unsigned long hash_create(uint64_t (*hash_function)(uint64_t const *, size_t)) {
	return 0;
}

void hash_delete(unsigned long id) {
	
}

size_t hash_size(unsigned long id) {
	return 0;
}

bool hash_insert(unsigned long id, uint64_t const * seq, size_t size) {
	return true;
}

bool hash_remove(unsigned long id, uint64_t const * seq, size_t size) {
	return true;
}

void hash_clear(unsigned long id) {
	
}

bool hash_test(unsigned long id, uint64_t const * seq, size_t size) {
	return true;
}
