// iterator flag only
#ifndef LWE_STL_ITERATOR
#define LWE_STL_ITERATOR

#include "../../base/base.h"

LWE_BEGIN
namespace stl {

enum Mod {
    FWD  = 1, //!< iterator
    BWD  = 2, //!< reverse iterator
    VIEW = 4, //!< const iterator
};
constexpr Mod operator|(Mod lhs, Mod rhs) {
    return static_cast<Mod>(int(lhs) | int(rhs));
}

template<Mod, class T> class Iterator;

} // namespace stl
LWE_END
#endif