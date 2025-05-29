#ifndef LWE_META_TYPE
#define LWE_META_TYPE

#include "../../core/core.h"

LWE_BEGIN
namespace meta {

/// unused type
enum class Registered : bool {
    REGISTERED = 1
};

/// @brief type codes
enum class EType : uint8_t {
    UNREGISTERED,
    VOID,
    SIGNED_INT,
    SIGNED_CHAR,
    SIGNED_SHORT,
    SIGNED_LONG,
    SIGNED_LONG_LONG,
    UNSIGNED_SHORT,
    UNSIGNED_INT,
    UNSIGNED_CHAR,
    UNSIGNED_LONG,
    UNSIGNED_LONG_LONG,
    BOOL,
    CHAR,
    WCHAR_T,
    FLOAT,
    DOUBLE,
    LONG_DOUBLE,
    ENUM,
    CLASS,
    UNION,
    POINTER,
    REFERENCE,
    FUNCTION,
    STD_STRING,
    STL_DEQUE,
    CONST,
};

//! @brief type info
struct Type {
public:
    template<typename T> static const Type& reflect();

public:
    Type() = default;
    Type(const Type&);
    Type(Type&&) noexcept;
    Type(EType);
    ~Type();
    Type& operator=(const Type&);
    Type& operator=(Type&&) noexcept;

public:
    //! @note [0] is main type
    //! e.g.
    //! [0] DEQUE [1] INT == Deque<int> 
    const EType& operator[](size_t) const;
    const EType* begin() const;
    const EType* end() const;
    size_t       size() const;
    hash_t       hash() const;
    EType        type() const;

public:
    //! @brief IS TO STRING NOT DEREFERENCE
    const char* operator*() const;

public:
    explicit operator string() const;
    operator EType() const;

private:
    static constexpr size_t STACK = (sizeof(size_t) + sizeof(EType*));

private:
    void                             shrink();
    void                             push(EType);
    template<typename T> static void reflect(Type*);

private:
    hash_t hashed = 0;
    size_t count  = 0;
    union {
        struct {
            EType* heap;
            size_t capacitor;
        };
        EType stack[STACK] = { EType::UNREGISTERED };
    };
};

}
LWE_END
#endif