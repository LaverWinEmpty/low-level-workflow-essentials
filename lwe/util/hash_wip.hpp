#ifndef LWE_UTIL_HASH
#define LWE_UTIL_HASH

#include "../base/base.h"
#include "../mem/block.hpp"

#include "iostream"

/*
    Hash for 64bit fixed

    Specilaization 
    - Recommended
      - inheritence `Hash<void>`
      - constructor with set `value`
      - constructor single argument
    ```
    template<typename T> struct Hash: Hash<void> {
        Hash(const T& in) {
            value = f(in);
        }
        // Delegating constructor can be utilized
        Hash(T&& in): Hash<void>(f(in)) { }
    };
    ```
*/

LWE_BEGIN
namespace container {
template<typename K, typename V> struct Record;
} // namespace container

namespace util {

template<typename T> struct Hash;

// default interface
template<> class Hash<void> {
    using StringProxy = mem::Block<17>;

protected:
    Hash(hash_t); //!< for dervied

public:
    template<typename T> Hash(const T&); //!< default hash
    operator hash_t() const;             //!< get hash
    StringProxy operator*() const;       //!< to string

protected:
    static hash_t fnv1a(const void*, size_t); //!< default

protected:
    hash_t value;
};

template<typename T> struct Hash: Hash<void> {
    using Hash<void>::Hash;
};

} // namespace util
LWE_END

#include "hash.ipp"
#endif
