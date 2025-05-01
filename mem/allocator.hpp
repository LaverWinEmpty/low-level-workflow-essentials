#ifndef LWE_MEM_ALLOCATOR
#define LWE_MEM_ALLOCATOR

#include "pool.hpp"

LWE_BEGIN

namespace mem {
using namespace config;
using namespace common;

//! @brief locked memory pool
class Allocator: public Pool {
public:
    template<typename T, size_t ALIGN = DEF_ALIGN, size_t COUNT = DEF_COUNT> class Statics {
        DECLARE_NO_COPY(Statics);

    public:
        Statics()  = default;
        ~Statics() = default;

    public:
        template<typename... Args> static T* allocate(Args&&...) noexcept;
        template<typename U> static bool     deallocate(U*) noexcept;
        static size_t                        release() noexcept;
        static size_t                        generate(size_t) noexcept;
        static Allocator*                    instance() noexcept;

    private:
        static Allocator allocator;
    };

public:
    class Manager {
    public:
        ~Manager();
    public:
        static Allocator* instance(size_t in);
    private:
        static std::unordered_map<size_t, Allocator*> map;
        static thread::Lock                           lock;
    };

public:
    //! @brief overloading: non virtual override
    template<typename T = void, typename... Args> T* allocate(Args&&...) noexcept;

    //! @brief overloading: non virtual override
    template<typename T> bool deallocate(T*) noexcept;

    //! @brief overloading: non virtual override
    size_t generate(size_t) noexcept;

    //! @brief overloading: non virtual override
    size_t release() noexcept;

public:
    Allocator(size_t chunk, size_t align = DEF_ALIGN, size_t count = DEF_COUNT) noexcept;
    ~Allocator() noexcept = default;

private:
    thread::Lock lock;
};

} // namespace mem
LWE_END

#include "allocator.ipp"
#endif