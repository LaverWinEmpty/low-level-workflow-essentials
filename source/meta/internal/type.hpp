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
enum class Keyword : uint8_t {
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
    Type(Keyword);
    ~Type();
    Type& operator=(const Type&);
    Type& operator=(Type&&) noexcept;

public:
    //! @note [0] is main type
    //! e.g.
    //! [0] DEQUE [1] INT == Deque<int> 
    const Keyword& operator[](size_t) const;
    const Keyword* begin() const;
    const Keyword* end() const;
    size_t         count() const; //!< type code count
    hash_t         hash() const;  //!< hashed data
    Keyword        type() const;  //!< get first type code, exclude const

public:
    //! @brief IS TO STRING NOT DEREFERENCE
    const char* operator*() const;

public:
    explicit operator string() const;
    operator Keyword() const;

private:
    static constexpr size_t STACK = (sizeof(size_t) + sizeof(Keyword*));

private:
    void                             shrink();
    void                             push(Keyword);
    template<typename T> static void reflect(Type*);

private:
    hash_t hashed  = 0;
    size_t counter = 0;
    union {
        struct {
            Keyword* heap;
            size_t capacitor;
        };
        Keyword stack[STACK] = { Keyword::UNREGISTERED };
    };
};

}
LWE_END
#endif