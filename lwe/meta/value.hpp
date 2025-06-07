#ifndef LWE_META_VALUE
#define LWE_META_VALUE

#include "type.hpp"
#include "../diag/expected.hpp"

LWE_BEGIN
namespace meta {

//! @brief enum container
template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>> struct Value {
    using U = std::make_unsigned_t<std::underlying_type_t<E>>;

public:
    //! @brief get enum value by index
    static const Enumerator& meta(size_t);

public:
    //! @brief get enum type info
    static const Type& type();

public:
    Value() noexcept = default;
    Value(E) noexcept;

public:
    Value(U) noexcept;

public:
    Value(const Value&) noexcept;
    Value(Value&&) noexcept = default;

public:
    Value& operator=(const Value&) noexcept;
    Value& operator=(Value &&) noexcept = default;

public:
    //! @brief this AND enum other
    Value operator&(E) const noexcept;

public:
    //! @brief this OR enum other
    Value operator|(E) const noexcept;

    //! @brief this XOR enum other
    Value operator^(E) const noexcept;

public:
    //! @brief this AND enum other
    Value& operator&=(E) noexcept;

public:
    //! @brief this AND enum other
    Value& operator|=(E) noexcept;

public:
    //! @brief this XOR enum other
    Value& operator^=(E) noexcept;

public:
    //! @brief this NOT
    Value operator~() const noexcept;

public:
    //! @brief this NOT
    bool  operator!() const noexcept;

public:
    //! @brief to string
    const char* operator*() const noexcept;

public:
    operator E() const noexcept;
    operator U() const noexcept;

private:
    U value = static_cast<E>(0);

// info
private:
    static const Enum* info;
};

}
LWE_END
#include "value.ipp"
#endif