LWE_BEGIN
namespace mem {

template<size_t SIZE, size_t ALIGN> Pool        Allocator<mem::Block<SIZE>, ALIGN>::pool(SIZE, ALIGN);
template<size_t SIZE, size_t ALIGN> async::Lock Allocator<mem::Block<SIZE>, ALIGN>::lock;

template<typename T, size_t A>
template<typename... Args>
T* Allocator<T, A>::allocate(Args&&... in) noexcept {
    void* ptr = nullptr;
    LOCKGUARD(Adapter::lock) {
        ptr = Adapter::pool.allocate<void>();
    }
    if constexpr(!std::is_void_v<T>) {
        new(ptr) T(std::forward<Args>(in)...);
    }
    return static_cast<T*>(ptr);
}

template<typename T, size_t A>
bool Allocator<T, A>::deallocate(T* in) noexcept {
    if constexpr(!std::is_void_v<T>) {
        in->~T();
    }
    LOCKGUARD(Adapter::lock) return Adapter::pool.deallocate(in);
}

template<typename T, size_t A> size_t Allocator<T, A>::release() noexcept {
    size_t cnt = 0;
    LOCKGUARD(Adapter::lock) {
        cnt = Adapter::pool.release();
    }
    return cnt;
}

template<typename T, size_t A> size_t Allocator<T, A>::generate(size_t in) noexcept {
    for(size_t i = 0; i < in; ++i) {
        bool check = false;
        LOCKGUARD(Adapter::lock) {
            if(!Adapter::pool.generate(1)) {
                return i;
            }
        }
    }
    return in;
}

} // namespace mem
LWE_END
