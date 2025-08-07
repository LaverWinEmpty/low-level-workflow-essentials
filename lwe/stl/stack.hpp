#ifndef LWE_STL_STACK
#define LWE_STL_STACK

#include "../meta/meta.h"
#include "../container/linear_buffer.hpp"

LWE_BEGIN
namespace stl {

DECLARE_CONTAINER((typename T, size_t SVO = 0), Stack, LWE::container::LinearBuffer, T, SVO);
REGISTER_CONTAINER((typename T, size_t SVO), Stack, Keyword::STL_STACK, T, SVO);

} // namespace stl
LWE_END
#endif
