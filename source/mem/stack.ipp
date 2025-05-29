#include "stack.hpp"

LWE_BEGIN
namespace mem {

template<typename T, size_t N, typename Enable> Stack<T, N, Enable>::operator T*() const noexcept {
    return const_cast<T*>(mem);
}

template<typename T, size_t N> Stack<T, N, std::enable_if_t<N == 0>>::operator T*() const noexcept {
    return nullptr;
}

} // namespace mem
LWE_END