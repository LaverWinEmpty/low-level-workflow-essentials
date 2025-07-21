#ifndef LWE_STL_VECTOR
#define LWE_STL_VECTOR

#include "../meta/meta.h"
#include "../container/ring_buffer.hpp"

LWE_BEGIN
namespace stl {

DECLARE_CONTAINER((typename T, size_t SVO = 0), Vector, LWE::container::RingBuffer, T, SVO);
REGISTER_CONTAINER((typename T, size_t SVO), Vector, Keyword::STL_VECTOR, T, SVO);

} // namespace stl
LWE_END
#endif
