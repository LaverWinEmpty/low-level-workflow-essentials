#ifndef LWE_UTIL_BUFFER
#define LWE_UTIL_BUFFER

#include "../base/base.h"

LWE_BEGIN
namespace util {

// static array
template<size_t N, typename T = char> struct Buffer {
public:
    constexpr size_t   count() const noexcept { return N; }
    constexpr size_t   size() const noexcept { return N * sizeof(T); }
    constexpr T*       data() noexcept { return buffer; }
    constexpr const T* data() const noexcept { return buffer; }
    constexpr T*       begin() noexcept { return buffer; }
    constexpr const T* begin() const noexcept { return buffer; }
    constexpr T*       end() noexcept { return buffer + N; }
    constexpr const T* end() const noexcept { return buffer + N; }

public:
    // uninit
    constexpr Buffer() = default;

public:
    template<typename = std::enable_if_t<sizeof(T) == sizeof(char)>>
    constexpr Buffer(const char* in) {
        size_t len = std::strlen(in);
        len        = len >= N ? N : len;
        std::copy_n(in, len, buffer);
        buffer[len] = T(0);
    }

public:
    template<typename... Args> constexpr Buffer(Args&&... args): buffer{ std::forward<Args>(args)... } {
        static_assert(sizeof...(args) <= N);
    }

public:
    template<typename U> constexpr U as() noexcept {
        static_assert(std::is_pointer_v<U> && sizeof(std::remove_pointer_t<U>) <= (N * sizeof(T)));
        return reinterpret_cast<U>(buffer);
    }
    template<typename U> constexpr const U as() const noexcept { return const_cast<Buffer*>(this)->as<U>(); }

public:
    constexpr T&       operator[](size_t index) noexcept { return buffer[index]; }
    constexpr const T& operator[](size_t index) const noexcept { return buffer[index]; }

private:
    T buffer[N];
};

} // namespace util
LWE_END
#endif
