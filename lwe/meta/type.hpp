#ifndef LWE_META_TYPE
#define LWE_META_TYPE

#include "internal/feature.hpp"

LWE_BEGIN
namespace meta {

/**************************************************************************************************
 * type info object
 **************************************************************************************************/

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
    Keyword        code() const;  //!< get first type code, exclude const

public:
    //! @brief IS TO STRING NOT DEREFERENCE
    const char* operator*() const;

public:
    bool operator==(const Type&) const;
    bool operator!=(const Type&) const;
    bool operator==(Keyword) const;
    bool operator!=(Keyword) const;

public:
    explicit operator string() const;
    explicit operator Keyword() const;

private:
    static constexpr size_t STACK = (sizeof(size_t) + sizeof(Keyword*));

private:
    void                             shrink();
    void                             push(Keyword);
    template<typename T> static void reflect(Type*);

private:
    static size_t stringify(string*, const Type&, size_t);

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

private:
    const char* str; //!< to string result cache
};

/**************************************************************************************************
 * metadata objects
 **************************************************************************************************/

//! @brief enum info
struct Enumerator {
    uint64_t    value;
    const char* name;
};

//! @brief member variable info
struct Field {
    Type        type;
    const char* name;
    size_t      size;
    size_t      offset;

    explicit operator bool() const { return name != nullptr; }
};

/// @brief method metadata;
/// @note  constructor, destructor, copy, move is use default
struct Signature {
    Type              ret;  //!< return type
    std::vector<Type> args; //!< argumnet types
};

class Object;
//! @brief class metadata
struct Class {
    virtual const char*      name() const    = 0;
    virtual size_t           size() const    = 0;
    virtual const Structure& fields() const  = 0;
    virtual const Class*     base() const    = 0;
    virtual const Object*    statics() const = 0;

public:
    const Field& field(const char*) const;
    const Field& field(const string&) const;
};

//! @brief enum metadata
struct Enum {
    virtual const char*        name() const  = 0;
    virtual size_t             size() const  = 0;
    virtual const Enumeration& enums() const = 0;

public:
    template<typename E> static const char* serialize(E);
    static const char*                      serialize(const string&, uint64_t);
    static const char*                      serialize(const char*, uint64_t);

public:
    template<typename E> static E deserialize(const char*);
    template<typename E> static E deserialize(const string&);
    static uint64_t               deserialize(const string&, const string&);
};

/**************************************************************************************************
 * util functions
 **************************************************************************************************/
template<typename T> const char* typestring();            //!< reflect type name string explicit
template<typename T> const char* typestring(const T&);    //!< reflect type name string implicit
const char*                      typestring(const Type&); //!< reflect type name

template<typename T> const Type& typeof();         //!< reflect typeinfo by template
template<typename T> const Type& typeof(const T&); //!< reflect typeinfo by argument

template<typename T> Class* classof();              //!< get class field list
template<typename T> Class* classof(const T&);      //!< get class field list
Class*                      classof(const char*);   //!< get class field list
Class*                      classof(const string&); //!< get class field list

template<typename T> Enum* enumof();              //!< get enum value list
template<typename T> Enum* enumof(const T&);      //!< get enum value list
Enum*                      enumof(const char*);   //!< get enum value list
Enum*                      enumof(const string&); //!< get enum value list

template<typename T> Object* statics();              //!< get static class
template<typename T> Object* statics(const T&);      //!< get static class
Object*                      statics(const char*);   //!< get static class
Object*                      statics(const string&); //!< get static class
}
LWE_END
#include "type.ipp"
#endif