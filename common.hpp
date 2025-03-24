
#ifndef LWE_COMMON
#define LWE_COMMON

#include "hal.hpp "

LWE_BEGIN

namespace common {

//! @brief check power of 2
inline constexpr bool aligned(uint64 in) noexcept {
    return in && !(in & (in - 1));
}

/**
 * @brief get next power of 2
 *
 * @param [in] in source value
 * @return power of 2: e.g. align(129) ->  256
 */
inline static constexpr uint64 align(uint64 in) noexcept {
    if(in <= 1) {
        return 1;
    }
    in -= 1;
    for(uint64 i = 1; i < sizeof(uint64); i <<= 1) {
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
inline static constexpr uint64 align(uint64 in, uint64 unit) noexcept {
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
template<typename T = void> inline T* memalloc(size_t size, size_t alignment = 0) noexcept {
    void* ptr = nullptr;
    void* out = nullptr;

    if(alignment <= 1) {
        ptr = malloc(size + sizeof(void*)); // add space for metadata;
        out = ptr;
    }

    else {
        alignment = align(alignment);       // defalut: 0 -> 1
        size      = align(size, alignment); // follow the POSIX standards

        ptr = malloc(size + alignment + sizeof(void*));
        if(ptr) {
            out = reinterpret_cast<void*>(align(reinterpret_cast<uintptr>(ptr), alignment)); // aligned pointer
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

} // namespace common

LWE_END
#endif