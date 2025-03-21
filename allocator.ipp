#include "allocator.hpp"

LWE_BEGIN
namespace MEM {

template<typename T, size_t ALIGN, size_t COUNT>
Allocator typename Allocator::Statics<T, ALIGN, COUNT>::allocator{ sizeof(T), ALIGN, COUNT };

// inline static Allocator allocator = Allocator(sizeof(T), A, C);

Allocator::Allocator(size_t chunk, size_t align, size_t count) noexcept: Pool(chunk, align, count) {}

template<typename T, typename... Args> T* Allocator::allocate(Args&&... in) noexcept {
    void* ptr = nullptr;
    LOCKGUARD(lock) {
        ptr = Allocator::allocate<void>();
    }
    if constexpr(!std::is_void_v<T>) {
        new(ptr) T(std::forward<Args>(in)...);
    }
    return static_cast<T*>(ptr);
}

template<typename T> bool Allocator::deallocate(T* in) noexcept {
    if constexpr(!std::is_void_v<T>) {
        in->~T();
    }
    LOCKGUARD(lock) return Allocator::deallocate(in);
}

size_t Allocator::generate(size_t in) noexcept {
    for(size_t i = 0; i < in; ++i) {
        bool check = false;
        LOCKGUARD(lock) {
            if(!Pool::generate()) {
                return i;
            }
        }
    }
    return in;
}

size_t Allocator::release() noexcept {
    size_t i = 0;
    while(freeable.head != nullptr) {
        LOCKGUARD(lock) {
            Block* ptr = freeable.dequeue();
            all.erase(ptr);
            memfree(ptr);
            counter.generated -= 1;
        }
    }
    return i;
}

template<typename T, size_t A, size_t C>
template<typename... Args>
T* Allocator::Statics<T, A, C>::allocate(Args&&... in) noexcept {
    return allocator.allocate<T>(std::forward<Args>(in)...);
}

template<typename T, size_t A, size_t C>
template<typename U>
bool Allocator::Statics<T, A, C>::deallocate(U* in) noexcept {
    return allocator.deallocate(static_cast<T*>(in));
}

template<typename T, size_t A, size_t C> size_t Allocator::Statics<T, A, C>::release() noexcept {
    return allocator.release();
}

template<typename T, size_t A, size_t C> size_t Allocator::Statics<T, A, C>::generate(size_t in) noexcept {
    return allocator.generate(in);
}

template<typename T, size_t A, size_t C> Allocator* Allocator::Statics<T, A, C>::instance() noexcept {
    return &allocator;
}

Allocator::Manager::~Manager() {
    for(auto& it : map) {
        delete it.second;
    }
}

Allocator* Allocator::Manager::instance(size_t size) {
    if(map.find(size) == map.end()) {
        // double-checked
        LOCKGUARD(lock) {
            if(map.find(size) == map.end()) {
                map[size] = new Allocator(size);
            }
        }
    }
    return map[size];
}

std::unordered_map<size_t, Allocator*> Allocator::Manager::map;
Thread::Lock                           Allocator::Manager::lock;

} // namespace MEM
LWE_END