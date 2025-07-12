#ifndef LWE_UTIL_HASH
#define LWE_UTIL_HASH

#include "../base/base.h"
#include "../mem/block.hpp"

LWE_BEGIN
namespace container {
template<typename K, typename V> struct Record;
} // namespace container

namespace util {

// specializationable
template<typename T = void> struct Hasher {
    hash_t operator()(const T& in) const {
        if constexpr(std::is_same_v<String, T> || std::is_same_v<StringView, T>) {
            return fnv1a(in.data(), in.size());
        }
        else if constexpr(std::is_same_v<const char*, T>) {
            return fnv1a(in, std::strlen(in));
        }
        else if constexpr(std::is_integral_v<T>) {
            return hash_t(in);
        }
        if constexpr(std::is_floating_point_v<T>) {
            if(in == -0) in = 0;
        }
        return fnv1a(&in, sizeof(T));
    }

private:
    //! @brief default
    static hash_t fnv1a(const void* in, size_t n) {
        static constexpr uint64_t FNV1A64_BASIS = 14'695'981'039'346'656'037ULL;
        static constexpr uint64_t FNV1A64_PRIME = 1'099'511'628'211ULL;

        hash_t         val = FNV1A64_BASIS;
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(in);
        for(size_t i = 0; i < n; i++) {
            val ^= ptr[i];
            val *= FNV1A64_PRIME;
        }
        return val;
    }
};

template<typename T> class Hash {
    using StringProxy = mem::Block<17>;

public:
    Hash(const T& in): val(Hasher<T>{}(in)) { }

public:
    operator hash_t() const { return val; }

public:
    StringProxy operator*() const {
        StringProxy out;
        snprintf(out.data(), 17, "%016llX", val);
        return out;
    }

private:
    hash_t val = 0;
};

template<typename K, typename V> struct hash<LWE::util::container::Record<K, V>> {
    std::size_t operator()(const LWE::util::container::Record<K, V>&) const;
};

} // namespace util
LWE_END

// {
//     size_t operator() { return }
// }

// #include "hash.ipp"
#endif
