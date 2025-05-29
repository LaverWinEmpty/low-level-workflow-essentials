#ifndef LWE_UTIL_HASH
#define LWE_UTIL_HASH

#include "../core/core.h"

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
        const char* operator*() const; //!< get string (c_str)
    
    public:
        operator hash_t() const; //!< get value
    
    private:
        hash_t val     = 0;
        char   str[17] = { 0 };
    };

}
LWE_END

#include "hash.ipp"
#endif