#ifndef LWE_MEM_BLOCK
#define LWE_MEM_BLOCK

#include "../diag/diag.h"

LWE_BEGIN
namespace mem {

template<size_t N, typename T = char> struct Block {
    Block() noexcept               = default;
    Block(const Block&)            = default;
    Block(Block&&)                 = default;
    ~Block()                       = default;
    Block& operator=(const Block&) = default;
    Block& operator=(Block&&)      = default;

public:
    constexpr Block(const T*, size_t);
    constexpr Block(const std::initializer_list<T>&);

private:
    template<typename U> using IsChar = std::enable_if_t<sizeof(U) == sizeof(char)>;
public:
    template<typename = IsChar<T>> constexpr Block(const String&);
    template<typename = IsChar<T>> constexpr Block(const char*);

public:
    constexpr T&       operator[](size_t) noexcept;
    constexpr const T& operator[](size_t) const noexcept;

public:
    constexpr operator T*() noexcept;
    constexpr operator const T*() const noexcept;

public:
    constexpr T&                            at(size_t);
    constexpr const T&                      at(size_t) const;
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
#include "block.ipp"
#endif
