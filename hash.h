/**
 * JNP1 2022/2023, Zadanie 2
 * Interfejs modułu, udostępniającego tablice haszujące
 * ciągi liczb typu uint64_t.
 *
 * Autorzy:
 * Ivan Gechu           ig439665
 * Tsimafei Lukashevich tl439668
 */

#ifndef JNP1_HASH_H
#define JNP1_HASH_H

#ifdef __cplusplus

#include <cstdlib>
#include <cstdint>
namespace jnp1 {
extern "C" {
using std::uint64_t;
using hash_function_t = uint64_t (*)(const uint64_t*, size_t);

#else

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
typedef uint64_t (*hash_function_t)(const uint64_t *, size_t);

#endif

/**
 * Tworzy tablicę haszującą i zwraca jej identyfikator. Parametr
 * hash_function jest wskaźnikiem na funkcję haszującą, która daje w wyniku
 * liczbę uint64_t i ma kolejno parametry const uint64_t* oraz size_t.
 */
unsigned long hash_create(hash_function_t hash_function);

/**
 * Usuwa tablicę haszującą o identyfikatorze id, o ile ona istnieje.
 * W przeciwnym przypadku nic nie robi.
 */
void hash_delete(unsigned long id);

/**
 * Daje liczbę ciągów przechowywanych w tablicy haszującej
 * o identyfikatorze id lub 0, jeśli taka tablica nie istnieje.
 */
size_t hash_size(unsigned long id);

/**
 * Wstawia do tablicy haszującej o identyfikatorze id ciąg liczb
 * całkowitych seq o długości size. Wynikiem jest informacja, czy operacja
 * się powiodła. Operacja się nie powiedzie, jeśli nie ma takiej tablicy
 * haszującej, jeśli tablica haszująca zawiera już taki ciąg, jeśli
 * parametr seq ma wartość NULL lub parametr size ma wartość 0.
 */
bool hash_insert(unsigned long id, const uint64_t* seq, size_t size);

/**
 * Usuwa z tablicy haszującej o identyfikatorze id ciąg liczb całkowitych
 * seq o długości size. Wynikiem jest informacja, czy operacja się
 * powiodła. Operacja się nie powiedzie, jeśli nie ma takiej tablicy
 * haszującej, jeśli tablica haszująca nie zawiera takiego ciągu,
 * jeśli parametr seq ma wartość NULL lub parametr size ma wartość 0.
 */
bool hash_remove(unsigned long id, const uint64_t* seq, size_t size);

/**
 * Jeśli tablica haszująca o identyfikatorze id istnieje i nie jest pusta,
 * to usuwa z niej wszystkie elementy. W przeciwnym przypadku nic nie robi.
 */
void hash_clear(unsigned long id);

/**
 * Daje wynik true, jeśli istnieje tablica haszująca o identyfikatorze id
 * i zawiera ona ciąg liczb całkowitych seq o długości size. Daje wynik
 * false w przeciwnym przypadku oraz gdy parametr seq ma wartość NULL lub
 * parametr size ma wartość 0.
 */
bool hash_test(unsigned long id, const uint64_t* seq, size_t size);


#ifdef __cplusplus

} /* extern "C"     */
} /* namespace jnp1 */

#endif

#endif /* JNP1_HASH_H */
