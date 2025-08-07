#ifndef LWE_STL_DEQUE
#define LWE_STL_DEQUE

#include "../meta/meta.h"
#include "../container/ring_buffer.hpp"

LWE_BEGIN
namespace stl {

DECLARE_CONTAINER((typename T, size_t SVO = 0), Deque, LWE::container::RingBuffer, T, SVO);
REGISTER_CONTAINER((typename T, size_t SVO), Deque, Keyword::STL_DEQUE, T, SVO);

} // namespace stl
LWE_END
#endif
