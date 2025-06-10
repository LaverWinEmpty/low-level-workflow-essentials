#ifndef LWE_UTIL_HASH
#define LWE_UTIL_HASH

#include "../base/base.h"
#include "buffer.hpp"

LWE_BEGIN
namespace util {
    
class Hash {
    static constexpr unsigned long long FNV1A64_BASIS = 14695981039346656037ULL;
    static constexpr unsigned long long FNV1A64_PRIME = 1099511628211ULL;

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
    operator string() const; //!< to string;

private:
    hash_t val = 0;
    static thread_local char buffer[17];
};


}
LWE_END

#include "hash.ipp"
#endif