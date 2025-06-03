
#ifndef LWE_CORE_HELPER
#define LWE_CORE_HELPER

#include "std.hpp"
#include "macro.hpp"

LWE_BEGIN
namespace core {

//! @brief check power of 2
inline constexpr bool aligned(uint64_t in) noexcept {
    return in && !(in & (in - 1));
}

/**
 * @brief get next power of 2
 *
 * @param [in] in source value
 * @return power of 2: e.g. align(129) ->  256
 */
inline static constexpr uint64_t align(uint64_t in) noexcept {
    if(in <= 1) {
        return 1;
    }
    in -= 1;
    for(uint64_t i = 1; i < sizeof(uint64_t); i <<= 1) {
        in |= in >> i;
    }
    return in + 1;
}

/**
 * @brief get padded value
 *
 * @param [in] in source value
 * @param [in] x  unit value
 * @return aligned value: e.g. align(11, 5) -> 15
 */
inline static constexpr uint64_t align(uint64_t in, uint64_t unit) noexcept {
    if(unit <= 1) {
        return in;
    }
    if(aligned(unit)) {
        return (in + unit - 1) & ~(unit - 1);
    }
    return (in + unit - 1) / unit * unit;
}

/**
 * @brief MSVC _aligned_malloc / POSIX aligned_alloc
 *
 * @param [in] size corrected to multiples of powers of 2
 * @param [in] alignment (optional) corrected to by powers of 2
 * @return void* allocated pointer
 */
template<typename T = void>  inline T* memalloc(size_t size, size_t alignment = 0) noexcept {
    void* ptr = nullptr; // real address
    void* out = nullptr; // user address

    if(alignment <= 1) {
        ptr = malloc(size + sizeof(void*)); // add space for metadata;
        out = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) + 1); // make space void*
    }

    else {
        alignment = align(alignment);       // defalut: 0 -> 1
        size      = align(size, alignment); // follow the POSIX standards

        ptr = malloc(size + alignment + sizeof(void*));
        if(ptr) {
            out = reinterpret_cast<void*>(align(reinterpret_cast<uintptr_t>(ptr), alignment)); // aligned pointer
            if(reinterpret_cast<char*>(out) - reinterpret_cast<char*>(ptr) < sizeof(void*)) {
                out = reinterpret_cast<char*>(out) + alignment; // make space for metadata pointer
            }
        }
    }

    if(out) {
        *(reinterpret_cast<void**>(out) - 1) = ptr;
        return reinterpret_cast<T*>(out);
    }
    return nullptr;
}

/**
 * @brief MSCV ::_aligned_free / POSIX free
 *
 * @param [in] in free pointer
 */
template<typename T> inline void memfree(T* in) noexcept {
    free(*(reinterpret_cast<void**>(in) - 1));
}

} // namespace core

LWE_END
#endif