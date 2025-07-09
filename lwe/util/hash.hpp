#ifndef LWE_UTIL_HASH
#define LWE_UTIL_HASH

#include "../base/base.h"
#include "../diag/diag.h"
#include "../stl/pair.hpp"

LWE_BEGIN
namespace container {
template<typename K, typename V> struct Record;
} // namespace container

namespace util {
class Hash {
    static constexpr unsigned long long FNV1A64_BASIS = 14'695'981'039'346'656'037ULL;
    static constexpr unsigned long long FNV1A64_PRIME = 1'099'511'628'211ULL;

public:
    //! string proxy
    struct String;

public:
    Hash(const void*, size_t);
    Hash(const char*);
    Hash(const string&);
    template<typename T> Hash(const T&);

public:
    bool operator==(const Hash&) const;
    bool operator!=(const Hash&) const;

public:
    operator hash_t() const; //!< get value

public:
    String operator*() const; //! get string adapter;

public:
    String stringify() const;

private:
    hash_t                   val = 0;
    static thread_local char buffer[17];
};

//! @brief type hash for specialization
template<typename T> hash_t hashof(const T& in) {
    return Hash(&in, sizeof(in));
}

//! @brief pair hash override
template<typename K, typename V> size_t hashof(const container::Record<K, V>&);

//! @brief pair hash override
template<typename K, typename V> size_t hashof(const stl::Pair<K, V>& in) {
    return hashof<K>(in.key);
}

// float
template<> hash_t hashof<float>(const float& in) {
    return hash_t(*reinterpret_cast<const uint32_t*>(&in));
}

// double
template<> hash_t hashof<double>(const double& in) {
    return hash_t(*reinterpret_cast<const uint64_t*>(&in));
}

// long double
template<> hash_t hashof<long double>(const long double& in) {
    if constexpr(sizeof(long double) == sizeof(uint64_t)) {
        return hash_t(*reinterpret_cast<const uint64_t*>(&in));
    }
    else if constexpr(sizeof(long double) > sizeof(uint64_t)) {
        return Hash(&in, sizeof(in)); // default hash
    }
    throw(diag::error(diag::TYPE_MISMATCH));
}

// clang-format off
template<> hash_t hashof<char>(const char& in) { return in; }
template<> hash_t hashof<bool>(const bool& in) { return in; }
template<> hash_t hashof<int8_t>(const int8_t& in) { return in; }
template<> hash_t hashof<int16_t>(const int16_t& in) { return in; }
template<> hash_t hashof<int32_t>(const int32_t& in) { return in; }
template<> hash_t hashof<int64_t>(const int64_t& in) { return in; }
template<> hash_t hashof<uint8_t>(const uint8_t& in) { return in; }
template<> hash_t hashof<uint16_t>(const uint16_t& in) { return in; }
template<> hash_t hashof<uint32_t>(const uint32_t& in) { return in; }
template<> hash_t hashof<uint64_t>(const uint64_t& in) { return in; }
// clang-format on

} // namespace util
LWE_END

#include "hash.ipp"
#endif
