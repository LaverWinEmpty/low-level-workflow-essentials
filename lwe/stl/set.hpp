#ifndef LWE_STL_SET
#define LWE_STL_SET

#include "../meta/meta.h"
#include "../container/hashtable.hpp"

LWE_BEGIN
namespace stl {

DECLARE_CONTAINER((typename T), Set, LWE::container::Hashtable, T);
REGISTER_CONTAINER((typename T), Set, STL_SET, T);

} // namespace stl
LWE_END
#endif
