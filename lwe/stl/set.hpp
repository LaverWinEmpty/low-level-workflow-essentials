#ifndef LWE_STL_SET
#define LWE_STL_SET

#include "../meta/meta.h"
#include "../container/hashed_buffer.hpp"

LWE_BEGIN
namespace stl {

DECLARE_CONTAINER((typename T), Set, LWE::container::HashedBuffer, T);
REGISTER_CONTAINER((typename T), Set, Keyword::TL_SET, T);

} // namespace stl
LWE_END
#endif
