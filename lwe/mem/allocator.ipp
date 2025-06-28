LWE_BEGIN
namespace mem {

//! specailizated pool for same-sized types
template<size_t SIZE, size_t ALIGN> class Allocator<Block<SIZE>, ALIGN> {
    using Type = Block<SIZE>;
public:
    template<typename... Args> static void* allocate(Args&&...) noexcept; //!< @return false: bad alloc
    static bool                             deallocate(void*) noexcept;   //!< @return false: failed
    static size_t                           generate(size_t) noexcept;    //!< @return succeeded count
    static size_t                           release() noexcept;           //!< @return succeeded count
private:
    static Pool        pool;
    static async::Lock lock;
};

template<size_t SIZE, size_t ALIGN> Pool        Allocator<Block<SIZE>, ALIGN>::pool(SIZE, ALIGN);
template<size_t SIZE, size_t ALIGN> async::Lock Allocator<Block<SIZE>, ALIGN>::lock;

template<size_t SIZE, size_t A>
template<typename... Args> void* Allocator<Block<SIZE>, A>::allocate(Args&&... in) noexcept {
    void* ptr = nullptr;
    LOCKGUARD(lock) {
        ptr = pool.allocate<void>();
    }
    if constexpr(!sizeof...(in)) {
        new(ptr) Type(std::forward<Args>(in)...);
    }
    return ptr;
}

template<size_t SIZE, size_t A> bool Allocator<Block<SIZE>, A>::deallocate(void* in) noexcept {
    LOCKGUARD(lock) {
        return pool.deallocate(in);
    }
}

template<size_t SIZE, size_t A> size_t Allocator<Block<SIZE>, A>::release() noexcept {
    size_t cnt = 0;
    LOCKGUARD(lock) {
        cnt = pool.release();
    }
    return cnt;
}

template<size_t SIZE, size_t A> size_t Allocator<Block<SIZE>, A>::generate(size_t in) noexcept {
    for(size_t i = 0; i < in; ++i) {
        bool check = false;
        LOCKGUARD(lock) {
            if(!pool.generate(1)) {
                return i;
            }
        }
    }
    return in;
}

template<typename T, size_t A>
template<typename... Args> T* Allocator<T, A>::allocate(Args&&... in) noexcept {
    void* ptr = Adapter::allocate();
    if (!ptr) {
        return nullptr; // bad alloc
    }

    // call constructor
    if constexpr(!std::is_void_v<T>) {
        new(ptr) T(std::forward<Args>(in)...);
    }
    return static_cast<T*>(ptr);
}

template<typename T, size_t A> bool Allocator<T, A>::deallocate(T* in) noexcept {
    if constexpr(!std::is_void_v<T>) {
        in->~T();
    }
    return Adapter::deallocate(in);
}

template<typename T, size_t A> size_t Allocator<T, A>::release() noexcept {
    return Adapter::release();
}

template<typename T, size_t A> size_t Allocator<T, A>::generate(size_t in) noexcept {
    return Adapter::generate(in);
}

} // namespace mem
LWE_END

