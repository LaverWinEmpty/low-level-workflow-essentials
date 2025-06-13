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
    static diag::Expected<Enumerator> find(uint64_t);      //! @brief get enum by value
    static diag::Expected<Enumerator> find(const char*);   //! @brief get enum by name
    static diag::Expected<Enumerator> find(const string&); //! @brief get enum by name
    static diag::Expected<Enumerator> get(size_t);         //! @brief get enum by index

public:
    //! @brief get enum value info (O(n))
    diag::Expected<Enumerator> meta();

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
    Value operator&(E) const noexcept; //!< AND
    Value operator|(E) const noexcept; //!< OR
    Value operator^(E) const noexcept; //!< XOR

public:
    Value& operator&=(E) noexcept;     //!< AND
    Value& operator|=(E) noexcept;     //!< OR
    Value& operator^=(E) noexcept;     //!< XOR

public:
    Value operator~() const noexcept; //!< NOT
    bool  operator!() const noexcept; //!< NOT bool

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