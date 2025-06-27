#ifndef LWE_SYS_EXCEPTIONAL
#define LWE_SYS_EXCEPTIONAL

#include "alert.hpp"
#include <type_traits>

LWE_BEGIN
namespace diag {

template<typename T> class Expected {
public:
    Expected(const Alert&) noexcept;
    Expected(Alert&&) noexcept;
    Expected(const T&) noexcept;
    Expected(T&&) noexcept;
    Expected(const Expected&) noexcept;
    Expected(Expected&&) noexcept;
    ~Expected();

public:
    Expected& operator=(const Expected&) noexcept;
    Expected& operator=(Expected&&) noexcept;

public:
    explicit operator bool() const;
    operator const Alert&() const;
    operator const T&() const;
    operator const char*() const;

public:
    T*       operator->();
    T&       operator*();
    const T* operator->() const;
    const T& operator*() const;

public:
    const char* what() const;  //!< error message, return "" when succeeded
    T&&         move();        //!< move data, throw when failed
    T&          data();        //!< get reference, throw when failed
    bool        valid() const; //!

private:
    union {
        Alert msg;
        T     res;
    };
    bool flag;
};

// reference specialize
template<typename T> class Expected<T&>: public Expected<std::remove_reference_t<T>*> {
    using Type = std::remove_reference_t<T>;
    using Base = Expected<Type*>;
public:
    Expected(Type& in): Base(&in) { }
    Expected(const Alert& in): Base(in) { }
    Expected(Alert&& in): Base(in) { }
    using Base::data;
    using Base::valid;
public:
    Type*       operator->() { return data(); }
    Type&       operator*() { return *data(); }
    const Type* operator->() const { return data(); }
    const Type& operator*() const { return *data(); }
};

} // namespace diag
LWE_END

#include "expected.ipp"
#endif
