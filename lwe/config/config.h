#ifndef LWE_CONFIG
#define LWE_CONFIG
#include "../base/base.h"

/**************************************************************************************************
 * configs
 *
 * override: #define SET_`value name`
 **************************************************************************************************/

LWE_BEGIN
namespace config {

//! container initial capacity (count)
inline constexpr size_t
#ifndef SET_CAPACITY
    CAPACITY = 4;
#else
    CAPACITY = align(SET_CAPACITY);
#endif
inline constexpr size_t CAPACITY_LOG = nlog(CAPACITY); // for hash table

//! memory pool allocate unit block (byte)
inline constexpr size_t
#ifndef SET_BLOCK
    BLOCK = 4'096;
#else
    BLOCK = align(SET_BLOCK, 1'024);
#endif

//! small vector optimize (byte)
inline constexpr size_t
#ifndef SET_SMALLVECTOR
    SMALLVECTOR = 64;
#else
    SMALLVECTOR = SET_SMALLVECTOR;
#endif

// default hash table load factor (ratio)
inline constexpr float
#ifndef SET_LOADFACTOR
    LOADFACTOR = 1.f;
#else
    LOADFACTOR = SET_LOADFACTOR;
#endif

} // namespace config
LWE_END
#endif
