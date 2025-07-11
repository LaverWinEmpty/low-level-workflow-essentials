#ifndef LWE_CONTAINER_DEQUE
#define LWE_CONTAINER_DEQUE

#include "../meta/meta.h"
#include "../container/deque.hpp"

LWE_BEGIN
namespace stl {

DECLARE_CONTAINER((typename T, size_t SVO = 0), Vector, LWE::container::Deque, T, SVO);
REGISTER_CONTAINER((typename T, size_t SVO), Vector, Keyword::STL_VECTOR, T, SVO);

} // namespace stl
LWE_END
#endif
