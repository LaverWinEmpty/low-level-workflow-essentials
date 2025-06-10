#ifdef LWE_MEM_ALLOCATOR
LWE_BEGIN
namespace mem {

template<size_t SIZE, size_t ALIGN> Pool        Allocator<util::Buffer<SIZE, int8_t>, ALIGN>::pool(SIZE, ALIGN);
template<size_t SIZE, size_t ALIGN> async::Lock Allocator<util::Buffer<SIZE, int8_t>, ALIGN>::lock;

template<size_t SIZE, size_t ALIGN>
template<typename T, typename... Args>
T* Allocator<util::Buffer<SIZE, int8_t>, ALIGN>::allocate(Args&&... in) noexcept {
    void* ptr = nullptr;
    LOCKGUARD(lock) {
        ptr = pool.allocate<void>();
    }
    if constexpr(!std::is_void_v<T>) {
        new(ptr) T(std::forward<Args>(in)...);
    }
    return static_cast<T*>(ptr);
}

template<size_t SIZE, size_t ALIGN>
template<typename T>
bool  Allocator<util::Buffer<SIZE, int8_t>, ALIGN>::deallocate(T* in) noexcept {
    if constexpr(!std::is_void_v<T>) {
        in->~T();
    }
    LOCKGUARD(lock) return pool.deallocate(in);
}

template<size_t SIZE, size_t ALIGN>
size_t Allocator<util::Buffer<SIZE, int8_t>, ALIGN>::generate(size_t in) noexcept {
    for(size_t i = 0; i < in; ++i) {
        bool check = false;
        LOCKGUARD(lock) {
            if(!pool.generate()) {
                return i;
            }
        }
    }
    return in;
}

template<size_t SIZE, size_t ALIGN>
size_t Allocator<util::Buffer<SIZE, int8_t>, ALIGN>::release() noexcept {
    size_t cnt = 0;
    LOCKGUARD(lock) {
        cnt = pool.release();
    }
    return cnt;
}

template<typename T, size_t A>
template<typename... Args>
T* Allocator<T, A>::allocate(Args&&... in) noexcept {
    return Adapter::template allocate<T>(std::forward<Args>(in)...);
}

template<typename T, size_t A>
bool Allocator<T, A>::deallocate(T* in) noexcept {
    return Adapter::template deallocate(static_cast<T*>(in));
}

template<typename T, size_t A> size_t Allocator<T, A>::release() noexcept {
    return Adapter::release();
}

template<typename T, size_t A> size_t Allocator<T, A>::generate(size_t in) noexcept {
    return Adapter::generate(in);
}

} // namespace MEM
LWE_END
#endif