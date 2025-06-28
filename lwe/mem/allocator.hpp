#ifndef LWE_MEM_ALLOCATOR
#define LWE_MEM_ALLOCATOR

#include "pool.hpp"
#include "../mem/block.hpp"

LWE_BEGIN

namespace mem {
using namespace config;

template<typename T, size_t ALIGN = DEF_ALIGN> class Allocator;

//! @brief helper to use the same pool when sizes are the same.
template<size_t SIZE, size_t ALIGN>
class Allocator<mem::Block<SIZE>, ALIGN> {
    using Type = mem::Block<SIZE>;

public:
    template<typename... Args> static void* allocate(Args&&...) noexcept; //!< @return false: bad alloc
    static bool                             deallocate(void*) noexcept;   //!< @return false: failed
    static size_t                           generate(size_t) noexcept;    //!< @return succeeded count
    static size_t                           release() noexcept;           //!< @return succeeded count

private:
    static Pool        pool;
    static async::Lock lock;
};

//! @brief default allocator
template<typename T, size_t ALIGN>
class Allocator {
    using Adapter = Allocator<mem::Block<core::align(sizeof(T), sizeof(void*))>, ALIGN>;

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
