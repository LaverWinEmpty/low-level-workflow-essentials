#ifndef LWE_CONTAINER_DEQUE
#define LWE_CONTAINER_DEQUE

#include "../meta/meta.h"
#include "../container/vector.hpp"

LWE_BEGIN
namespace stl {

DECLARE_CONTAINER((typename T, size_t SVO = 0), Deque, LWE::container::Vector, T, SVO);
REGISTER_CONTAINER((typename T, size_t SVO), Deque, STL_DEQUE, T, SVO);

} // namespace stl
LWE_END
#endif
