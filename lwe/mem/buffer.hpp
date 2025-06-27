#ifndef LWE_MEM_BUFFER
#define LWE_MEM_BUFFER

#include "../diag/diag.h"

LWE_BEGIN
namespace mem {

template<size_t N, typename T = char> struct Buffer {
    Buffer() noexcept                = default;
    Buffer(const Buffer&)            = default;
    Buffer(Buffer&&)                 = default;
    ~Buffer()                        = default;
    Buffer& operator=(const Buffer&) = default;
    Buffer& operator=(Buffer&&)      = default;

public:
    constexpr Buffer(const T*, size_t);
    constexpr Buffer(const std::initializer_list<T>&);

private:
    template<typename T> using IsChar = std::enable_if_t<sizeof(T) == sizeof(char)>;
public:
    template<typename = IsChar<T>> constexpr Buffer(const string&);
    template<typename = IsChar<T>> constexpr Buffer(const char*);

public:
    constexpr T&       operator[](size_t) noexcept;
    constexpr const T& operator[](size_t) const noexcept;

public:
    constexpr operator T*() noexcept;
    constexpr operator const T*() const noexcept;

public:
    constexpr diag::Expected<T&>            at(size_t) noexcept;
    constexpr diag::Expected<const T&>      at(size_t) const noexcept;
    constexpr T*                            data() noexcept;
    constexpr const T*                      data() const noexcept;
    template<typename U> constexpr U*       as() noexcept;
    template<typename U> constexpr const U* as() const noexcept;
    static constexpr size_t                 size() noexcept;

public:
    constexpr const T* begin() const noexcept;
    constexpr const T* end() const noexcept;
    constexpr T*       begin() noexcept;
    constexpr T*       end() noexcept;

private:
    T stack[N];
};

} // namespace mem
LWE_END
#endif
