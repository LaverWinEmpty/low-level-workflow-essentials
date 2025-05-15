#ifndef LWE_MEM_STACK
#define LWE_MEM_STACK

#include "../base/hal.hpp"

LWE_BEGIN

/**
 * @brief MEMORY stack, not container
 */
namespace mem {

// local array template util
template<typename T, size_t N, typename = void> struct Stack {
    operator T*() const noexcept;
private:
    T mem[N]; // declare if SVO is not 0
};

// size 0 specialization
template<typename T, size_t N> struct Stack<T, N, std::enable_if_t<N == 0>> {
    operator T*() const noexcept;
};

} // namespace mem
LWE_END
#include "stack.ipp"
#endif