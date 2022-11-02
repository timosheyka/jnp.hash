#include "hash.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cassert>

namespace {
    #ifdef NDEBUG
    const bool debug = false;
    #else
    const bool debug = true;
    #endif

    #define derr if (debug) get_cerr() << __func__

    using id_type = unsigned long;
    using hash_type = uint64_t;
    using seq_t = const uint64_t*;
    using hash_function_t = hash_type (*)(seq_t, size_t);
    using seq_type = std::vector<uint64_t>;
    using seq_vector = std::vector<seq_type>;
    using hash_table = std::unordered_map<hash_type, seq_vector>;
    using hash_tables_ht = std::unordered_map<id_type, hash_table>;
    using hash_functions_ht = std::unordered_map<id_type, hash_function_t>;
    using sizes_ht = std::unordered_map<id_type, size_t>;

    id_type counter = 0;

    std::ostream& get_cerr() {
        static std::ios_base::Init init;
        return std::cerr;
    }

    hash_tables_ht& hash_tables() {
        static hash_tables_ht* hash_tables = new hash_tables_ht;
        return *hash_tables;
    }

    hash_functions_ht& hash_functions() {
        static hash_functions_ht* hash_functions = new hash_functions_ht();
        return *hash_functions;
    }

    sizes_ht& sizes() {
        static sizes_ht* sizes = new sizes_ht();
        return *sizes;
    }

    bool hash_table_exists(id_type id) {
        return sizes().find(id) != sizes().end();
    }

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
            }
            str += "\"";
        }

        return str;
    }

    bool sequence_exists(const seq_vector& seq_vec, seq_type seq) {
        return std::find(seq_vec.begin(), seq_vec.end(), seq) != seq_vec.end();
    }

    bool check_errors(const std::string& function_name, id_type id,
                                seq_t seq, size_t size) {
        bool valid_input = true;

        if (seq == nullptr) {
            if (debug) {
                get_cerr() << function_name << ": invalid pointer (NULL)\n";
            }
            valid_input = false;
        }

        if (size == 0) {
            if (debug) {
                get_cerr() << function_name << ": invalid size (0)\n";
            }
            valid_input = false;
        }

        if (!valid_input) {
            return false;
        }

        if (!hash_table_exists(id)) {
            if (debug) {
                get_cerr() << function_name << ": hash table #" << id << " does not exist\n";
            }
            return false;
        }

        return true;
    }
}

namespace jnp1 {
    id_type hash_create(hash_function_t hash_function) {
        assert(hash_function != nullptr);

        derr << "(" << &hash_function << ")\n";

        hash_functions()[counter] = hash_function;
        sizes()[counter] = 0;

        derr << ": hash table #" << counter << " created\n";

        return counter++;
    }

    void hash_delete(id_type id) {
        derr << "(" << id << ")\n";

        if (hash_functions().erase(id) > 0) {
            hash_tables().erase(id);
            sizes().erase(id);

            derr << ": hash table #" << id << " deleted\n";
        } else {
            derr << ": hash table #" << id << " does not exist\n";
        }
    }

    size_t hash_size(id_type id) {
        derr << "(" << id << ")\n";

        if (!hash_table_exists(id)) {
            derr << ": hash table #" << id << " does not exist\n";
            return 0;
        }

        size_t size = sizes()[id];
        derr << ": hash table #" << id << " contains " << size << " element(s)\n";
        return size;
    }

    bool hash_insert(id_type id, seq_t seq, size_t size) {
        std::string seq_str;
        if (debug) {
            seq_str = seq_to_string(seq, size);
        }
        derr << "(" << id << ", " << seq_str << ", " << size << ")\n";

        if (!check_errors(__func__, id, seq, size)) {
            return false;
        }

        seq_type sequence(seq, seq + size);
        hash_type hash = hash_functions()[id](seq, size);
        hash_table& ht = hash_tables()[id];

        bool found = false;
        if (ht.find(hash) != ht.end()) {
            found = sequence_exists(ht[hash], sequence);
        }

        if (!found) {
            ht[hash].push_back(sequence);
            sizes()[id]++;
            derr << ": hash table #" << id << ", sequence " << seq_str << " inserted\n";
            return true;
        } else {
            derr << ": hash table #" << id << ", sequence " << seq_str << " was present\n";
            return false;
        }
    }

    bool hash_remove(id_type id, seq_t seq, size_t size) {
        std::string seq_str;
        if (debug) {
            seq_str = seq_to_string(seq, size);
        }
        derr << "(" << id << ", " << seq_str << ", " << size << ")\n";

        if (!check_errors(__func__, id, seq, size)) {
            return false;
        }

        seq_type sequence(seq, seq + size);
        hash_type hash = hash_functions()[id](seq, size);
        hash_table& ht = hash_tables()[id];

        if (ht.find(hash) != ht.end()) {
            seq_vector& seq_vec = ht[hash];
            auto it = std::find(seq_vec.begin(), seq_vec.end(), sequence);
            if (it != seq_vec.end()) {
                seq_vec.erase(it);
                if (seq_vec.empty()) {
                    ht.erase(hash);
                }
                sizes()[id]--;
                derr << ": hash table #" << id << ", sequence " << seq_str << " removed\n";
                return true;
            }
        }

        derr << ": hash table #" << id << ", sequence " << seq_str << " was not present\n";
        return false;
    }

    void hash_clear(id_type id) {
        derr << "(" << id << ")\n";

        if (!hash_table_exists(id)) {
            derr << ": hash table #" << id << " does not exist\n";
            return;
        }

        if (sizes()[id] == 0) {
            derr << ": hash table #" << id << " was empty\n";
            return;
        }

        hash_tables()[id].clear();
        sizes()[id] = 0;

        derr << ": hash table #" << id << " cleared\n";
    }

    bool hash_test(id_type id, seq_t seq, size_t size) {
        std::string seq_str;
        if (debug) {
            seq_str = seq_to_string(seq, size);
        }
        derr << "(" << id << ", " << seq_str << ", " << size << ")\n";

        if (!check_errors(__func__, id, seq, size)) {
            return false;
        }

        seq_type sequence(seq, seq + size);
        hash_type hash = hash_functions()[id](seq, size);
        hash_table& ht = hash_tables()[id];

        bool found = false;
        if (ht.find(hash) != ht.end()) {
            found = sequence_exists(ht[hash], sequence);
        }

        if (found) {
            derr << ": hash table #" << id << ", sequence " << seq_str << " is present\n";
        } else {
            derr << ": hash table #" << id << ", sequence " << seq_str << " is not present\n";
        }

        return found;
    }
}
