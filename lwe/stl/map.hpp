#ifndef LWE_STL_MAP
#define LWE_STL_MAP

#include "../meta/meta.h"
#include "../container/dictionary.hpp"

LWE_BEGIN
namespace stl {

DECLARE_CONTAINER((typename K, typename V), Map, LWE::container::Dictionary, K, V);
REGISTER_CONTAINER((typename K, typename V), Map, Keyword::STL_MAP, K, V);

} // namespace stl
LWE_END
#endif
