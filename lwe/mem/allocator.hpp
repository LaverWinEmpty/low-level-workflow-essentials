#ifndef LWE_MEM_ALLOCATOR
#define LWE_MEM_ALLOCATOR

#include "pool.hpp"
#include "../mem/block.hpp"

LWE_BEGIN

namespace mem {
using namespace config;

//! @brief default allocator
template<typename T, size_t ALIGN = DEF_ALIGN>
class Allocator {
    using Adapter = Allocator<Block<core::align(sizeof(T), sizeof(void*))>, ALIGN>;
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
