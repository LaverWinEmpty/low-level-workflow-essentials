#ifndef LWE_CONFIG
#define LWE_CONFIG
#include "../base/base.h"

/**************************************************************************************************
 * configs
 *
 * override: #define CONFIGDEF_`value name`_`type`
 **************************************************************************************************/

LWE_BEGIN
namespace config {

// array default allocate size (count)
inline constexpr size_t
#ifndef CONFIGDEF_ELEMENTCOUNT_SIZE
    ELEMENTCOUNT = 8;
#else
    ELEMENTCOUNT = CONFIGDEF_ELEMENTCOUNT_SIZE;
#endif


// memory pool allocation unit
inline constexpr size_t
#ifndef CONFIGDEF_MEMORYPAGE_SIZE
    MEMORYPAGE = 4'096;
#else
    MEMORYPAGE = CONFIGDEF_MEMORYPAGE_SIZE;
#endif

// deque small vector optimization (byte)
inline constexpr size_t
#ifndef CONFIGDEF_SMALLVECTOR_SIZE
    SMALLVECTOR = 64;
#else
    SMALLVECTOR = CONFIGDEF_SMALLVECTOR_SIZE;
#endif

// hash set load factor
inline constexpr float
#ifndef CONFIGDEF_LOADFACTOR_RATIO
    LOADFACTOR = 1.f;
#else
    LOADFACTOR = CONFIGDEF_LOADFACTOR_RATIO;
#endif

} // namespace config
LWE_END
#endif
