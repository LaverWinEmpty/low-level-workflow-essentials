/*
    Hash functional object
    - Stability: Enforced 64-bit design ensures identical results across platforms
    - Flexibility: Enhanced usability and specialization flexibility

    ? How to Specialization
    - Recommended
      1. inheritence `Hash<void>`
      2. constructor with set `value`
      3. constructor single argument
    ! But ignore when not using `Hashtable`
      This interface is for `Hashtable`
    e.g.
    ```
    template<typename T> struct LWE::util::Hash: Hash<void> {
        Hash(const T& in) {
            value = f(in);
        }
        // Delegating constructor can be utilized
        Hash(T&& in): Hash<void>(f(in)) { }
    };
    ```
*/

#ifndef LWE_UTIL_HASH
#define LWE_UTIL_HASH

#include "../base/base.h"
#include "../mem/block.hpp"

LWE_BEGIN

namespace util {

template<typename T = void> struct Hash;

// default interface
template<> class Hash<void> {
    using StringProxy = mem::Block<17>;

protected:
    Hash(hash_t); //!< for dervied

public:
    template<typename T> Hash(const T&); //!< ctor: default hash
    Hash() = default;                    //!< ctor: uninit
    operator hash_t() const;             //!< operator: get hash
    StringProxy operator*() const;       //!< operator: to string

public:
    /**************************************************************************
     * PREDEFIND HASH FUNCTIONS
     **************************************************************************/
    static hash_t fnv1a(const void*, size_t); //!< default

protected:
    hash_t value;
};

/**************************************************************************************************
 * DEFAULT TYPES
 **************************************************************************************************/
// clang-format off
Hash(signed int)         -> Hash<void>;
Hash(signed char)        -> Hash<void>;
Hash(signed short)       -> Hash<void>;
Hash(signed long)        -> Hash<void>;
Hash(signed long long)   -> Hash<void>;
Hash(unsigned int)       -> Hash<void>;
Hash(unsigned char)      -> Hash<void>;
Hash(unsigned short)     -> Hash<void>;
Hash(unsigned long)      -> Hash<void>;
Hash(unsigned long long) -> Hash<void>;
Hash(char)               -> Hash<void>;
Hash(float)              -> Hash<void>;
Hash(double)             -> Hash<void>;
Hash(long double)        -> Hash<void>;
Hash(const char*)        -> Hash<void>;
Hash(String)             -> Hash<void>;
Hash(StringView)         -> Hash<void>;
Hash(void*)              -> Hash<void>;
// clang-format on

// for unregistered type
template<typename T> struct Hash: Hash<void> {
    using Hash<void>::Hash;
};

} // namespace util
LWE_END

#include "hash.ipp"
#endif
