#ifndef LWE_MEM_ALLOCATOR
#define LWE_MEM_ALLOCATOR

#include "pool.hpp"
#include "../util/buffer.hpp"

LWE_BEGIN

namespace mem {
using namespace config;

template<typename T, size_t ALIGN = DEF_ALIGN> class Allocator;

//! @brief helper to use the same pool when sizes are the same.
template<size_t SIZE, size_t ALIGN>
class Allocator<util::Buffer<SIZE, int8_t>, ALIGN> {
    template<typename, size_t> friend class Allocator;
    static Pool        pool;
    static async::Lock lock;
};

//! @brief default allocator
template<typename T, size_t ALIGN>
class Allocator {
    using Adapter = Allocator<util::Buffer<core::align(sizeof(T), sizeof(void*)), int8_t>, ALIGN>;

public:
    template<typename... Args> static T* allocate(Args&&...) noexcept; //!< @return false: bad alloc
    static bool                          deallocate(T*) noexcept;      //!< @return false: failed
    static size_t                        generate(size_t) noexcept;    //!< @return succeeded count
    static size_t                        release() noexcept;           //!< @return succeeded count
};

} // namespace mem
LWE_END

#include "allocator.ipp"
#endif