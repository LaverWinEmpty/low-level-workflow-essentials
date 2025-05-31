#ifndef LWE_MEM_POOL
#define LWE_MEM_POOL

#include "../base/base.h"
#include "../sync/lock.hpp"
// #include "../stl/deque.hpp"

/*******************************************************************************
 * pool structure
 *
 * example configuration
 *  - chunk: 96
 *  - align: 32
 *  - count: 2
 *
 * 192                 256                384         512 | << address
 * ^                   ^                  ^           ^   |
 * +-------+----+------+------+----+------+------+----+   |
 * | block |    | meta | data |    | meta | data |    |   |
 * +-------+----+------+------+----+------+------+----+   |
 *         | 16 |             | 24 |             | 32 |   | << padding
 *              +--- chunk ---+    +--- chunk ---+
 *
 * total: 320 byte (64 + 128 * 2)
 *
 * - block : block header (struct) like node
 *   |-[ 8 byte]: next chunk pointer
 *   |-[ 8 byte]: next block pointer
 *   |-[ 8 byte]: prev block pointer
 *   |-[ 8 byte]: outer pool pointer
 *   |-[ 8 byte]: using chunk count
 *   +-[16 byte]: padding
 *
 * - chunk: not a struct, abstract object for dynamic chunk size.
 *   |-[ 8 byte]: outer block address (meta)
 *   |-[96 byte]: actual usable space  (data)
 *   +-[24 byte]: padding
 *
 * NOTE: align is intended for SIMD use and increases capacity.
 * block header padding reason: to ensures alignment for chunk start addresses.
 *
 ******************************************************************************/

LWE_BEGIN

namespace mem {
namespace config {
static constexpr size_t DEF_ALIGN = sizeof(void*);
static constexpr size_t DEF_COUNT = 64;
} // namespace config
using namespace config;

class Pool : Noncopy {
protected:
    //! @brief memory pool block node
    //! @note  4 pointer = 32 byte in x64
    struct Block;

protected:
    //! @brief block handler
    struct Queue {
    public:
        void   enqueue(Block*) noexcept;
        Block* dequeue() noexcept;
        void   pop(Block*) noexcept;

    public:
        Block* head = nullptr;
        Block* tail = nullptr;
    } usable, freeable;

public:
    //! @brief block counter
    struct Counter {
        size_t generated; //!< block generated count
        size_t blocks;    //!< number of blocks in use
        size_t chunks;    //!< number of chunks in use
    } counter;

public:
    /**
     * @brief construct a new pool object
     *
     * @param [in] chunk - chunk size, it is padded to the pointer size.
     * @param [in] align - chunk align, it is adjusted to the power of 2.
     * @param [in] count - chunk count in block.
     */
    Pool(size_t chunk, size_t alignment = DEF_ALIGN, size_t count = DEF_COUNT) noexcept;

public:
    //! @brief destroy the pool object.
    ~Pool() noexcept;

public:
    //! @brief get memory, call malloc when top is null and garbage collector is empty.
    template<typename T = void, typename... Args> T* allocate(Args&&...) noexcept;

public:
    //! @brief return memory, if not child of pool, push to garbage collector of outer pool.
    template<typename T = void> bool deallocate(T*) noexcept;

protected:
    //! @brief generate block.
    bool generate() noexcept;

public:
    //! @brief generate blocks.
    size_t generate(size_t size) noexcept;

public:
    //! @brief free all freeable blocks
    size_t release() noexcept;

public:
    //! @brief get pool state
    Counter count() const noexcept;

protected:
    const size_t ALIGN; //!< chunk alignment
    const size_t CHUNK; //!< chunk size
    const size_t COUNT; //!< chunk count
    const size_t META;  //!< block header size

protected:
    std::unordered_set<Block*> all; //!<  generated blocks
};

struct Pool::Block {
    void  initialize(Pool*, size_t) noexcept;
    void* get() noexcept;
    void  set(void*) noexcept;
    bool  full() const noexcept;
    bool  empty() const noexcept;

    static Block* find(void*) noexcept;

    Pool*  from;
    void*  curr;
    Block* next;
    Block* prev;
    size_t used;
};

} // namespace mem
LWE_END

#include "pool.ipp"
#endif