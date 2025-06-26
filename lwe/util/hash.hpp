#ifndef LWE_UTIL_HASH
#define LWE_UTIL_HASH

#include "../base/base.h"
#include "buffer.hpp"

LWE_BEGIN
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

} // namespace util
LWE_END

#include "hash.ipp"
#endif
