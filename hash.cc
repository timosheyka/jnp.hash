/**
 * JNP1 2022/2023, Zadanie 2
 * Implementacja modułu, udostępniającego tablice haszujące
 * ciągi liczb typu uint64_t.
 *
 * Autorzy:
 * Ivan Gechu           ig439665
 * Tsimafei Lukashevich tl439668
 */

#include "hash.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cassert>

#define derr if (debug) get_cerr() << __func__

namespace jnp1 {
    namespace {
#ifdef NDEBUG
        const bool debug = false;
#else
        const bool debug = true;
#endif

        using id_type = unsigned long;
        using seq_t = const uint64_t*;
        using seq_type = std::vector<uint64_t>;
        using sequences = std::vector<seq_type>;
        using hash_type = uint64_t;
        using hash_table = std::unordered_map<hash_type, sequences>;
        using hash_tables_ht = std::unordered_map<id_type, hash_table>;
        using hash_functions_ht = std::unordered_map<id_type, hash_function_t>;
        using sizes_ht = std::unordered_map<id_type, size_t>;

        id_type counter = 0;

        std::ostream& get_cerr() {
            static std::ios_base::Init initializer;
            return std::cerr;
        }

        hash_tables_ht& get_hash_tables() {
            static hash_tables_ht* hash_tables = new hash_tables_ht;
            return *hash_tables;
        }

        hash_functions_ht& get_hash_functions() {
            static hash_functions_ht* hash_functions = new hash_functions_ht();
            return *hash_functions;
        }

        sizes_ht& get_sizes() {
            static sizes_ht* sizes = new sizes_ht();
            return *sizes;
        }

        /**
         * Sprawdza, czy istnieje tablica haszująca o identyfikatorze id.
         */
        bool hash_table_exists(id_type id) {
            return get_sizes().find(id) != get_sizes().end();
        }

        /**
         * Konwertuje ciąg liczb uint64_t do napisu i zwraca go.
         */
        std::string seq_to_string(seq_t seq, size_t size) {
            std::string str;

            if (seq == nullptr) {
                str = "NULL";
            } else {
                str = "\"";
                for (size_t i = 0; i < size; i++) {
                    if (i > 0) {
                        str += " ";
                    }
                    str += std::to_string(seq[i]);
                }                str += "\"";
            }

            return str;
        }

        /**
         * Sprawdza, czy istnieje ciąg seq w kontenerze ciągów seqs.
         */
        bool sequence_exists(const sequences& seqs, seq_type seq) {
            return std::find(seqs.begin(), seqs.end(), seq) != seqs.end();
        }

        /**
         * Sprawdza, czy nie ma błędów w parametrach, przekazanych do funkcji z
         * nazwą function_name (to może być jedna z hash_insert, hash_remove,
         * hash_test) i wypisuje odpowiednie komunikaty na standardowe wyjście
         * diagnostyczne, jeśli program był kompilowany w trybie debugowania.
         */
        bool check_errors(const std::string& function_name, id_type id,
                                                 seq_t seq, size_t size) {
            bool valid_input = true;

            if (seq == nullptr) {
                if (debug) {
                    get_cerr() << function_name << ": invalid pointer (NULL)"
                               << std::endl;
                }
                valid_input = false;
            }

            if (size == 0) {
                if (debug) {
                    get_cerr() << function_name << ": invalid size (0)"
                               << std::endl;
                }
                valid_input = false;
            }

            if (!valid_input) {
                return false;
            }

            if (!hash_table_exists(id)) {
                if (debug) {
                    get_cerr() << function_name << ": hash table #" << id
                               << " does not exist" << std::endl;
                }
                return false;
            }

            return true;
        }
    }

    id_type hash_create(hash_function_t hash_function) {
        assert(hash_function != nullptr);

        derr << "(" << &hash_function << ")" << std::endl;

        get_hash_functions()[counter] = hash_function;
        get_sizes()[counter] = 0;

        derr << ": hash table #" << counter << " created" << std::endl;

        return counter++;
    }

    void hash_delete(id_type id) {
        derr << "(" << id << ")" << std::endl;

        if (get_hash_functions().erase(id) > 0) {
            get_hash_tables().erase(id);
            get_sizes().erase(id);

            derr << ": hash table #" << id << " deleted" << std::endl;
        } else {
            derr << ": hash table #" << id << " does not exist" << std::endl;
        }
    }

    size_t hash_size(id_type id) {
        derr << "(" << id << ")" << std::endl;

        if (!hash_table_exists(id)) {
            derr << ": hash table #" << id << " does not exist" << std::endl;
            return 0;
        }

        size_t size = get_sizes()[id];
        derr << ": hash table #" << id << " contains " << size
             << " element(s)" << std::endl;
        return size;
    }

    bool hash_insert(id_type id, seq_t seq, size_t size) {
        std::string seq_str;
        if (debug) {
            seq_str = seq_to_string(seq, size);
        }
        derr << "(" << id << ", " << seq_str << ", " << size << ")" << std::endl;

        if (!check_errors(__func__, id, seq, size)) {
            return false;
        }

        seq_type sequence(seq, seq + size);
        hash_type hash = get_hash_functions()[id](seq, size);
        hash_table& ht = get_hash_tables()[id];

        bool found = false;
        if (ht.find(hash) != ht.end()) {
            found = sequence_exists(ht[hash], sequence);
        }

        if (!found) {
            ht[hash].push_back(sequence);
            get_sizes()[id]++;
            derr << ": hash table #" << id << ", sequence "
                 << seq_str << " inserted" << std::endl;
            return true;
        } else {
            derr << ": hash table #" << id << ", sequence "
                 << seq_str << " was present" << std::endl;
            return false;
        }
    }

    bool hash_remove(id_type id, seq_t seq, size_t size) {
        std::string seq_str;
        if (debug) {
            seq_str = seq_to_string(seq, size);
        }
        derr << "(" << id << ", " << seq_str << ", " << size << ")" << std::endl;

        if (!check_errors(__func__, id, seq, size)) {
            return false;
        }

        seq_type sequence(seq, seq + size);
        hash_type hash = get_hash_functions()[id](seq, size);
        hash_table& ht = get_hash_tables()[id];

        if (ht.find(hash) != ht.end()) {
            sequences& seqs = ht[hash];
            auto it = std::find(seqs.begin(), seqs.end(), sequence);
            if (it != seqs.end()) {
                seqs.erase(it);
                if (seqs.empty()) {
                    ht.erase(hash);
                }
                get_sizes()[id]--;
                derr << ": hash table #" << id << ", sequence "
                     << seq_str << " removed" << std::endl;
                return true;
            }
        }

        derr << ": hash table #" << id << ", sequence " << seq_str
             << " was not present" << std::endl;
        return false;
    }

    void hash_clear(id_type id) {
        derr << "(" << id << ")" << std::endl;

        if (!hash_table_exists(id)) {
            derr << ": hash table #" << id << " does not exist" << std::endl;
            return;
        }

        if (get_sizes()[id] == 0) {
            derr << ": hash table #" << id << " was empty" << std::endl;
            return;
        }

        get_hash_tables()[id].clear();
        get_sizes()[id] = 0;

        derr << ": hash table #" << id << " cleared" << std::endl;
    }

    bool hash_test(id_type id, seq_t seq, size_t size) {
        std::string seq_str;
        if (debug) {
            seq_str = seq_to_string(seq, size);
        }
        derr << "(" << id << ", " << seq_str << ", " << size << ")" << std::endl;

        if (!check_errors(__func__, id, seq, size)) {
            return false;
        }

        seq_type sequence(seq, seq + size);
        hash_type hash = get_hash_functions()[id](seq, size);
        hash_table& ht = get_hash_tables()[id];

        bool found = false;
        if (ht.find(hash) != ht.end()) {
            found = sequence_exists(ht[hash], sequence);
        }

        if (found) {
            derr << ": hash table #" << id << ", sequence "
                 << seq_str << " is present" << std::endl;
        } else {
            derr << ": hash table #" << id << ", sequence "
                 << seq_str << " is not present" << std::endl;
        }

        return found;
    }
}
