#ifndef LWE_META
#define LWE_META

#include "../common/hal.hpp"

LWE_BEGIN

namespace meta {

class Object;

/// @brief type codes
enum class EType : uint8 {
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

/// @brief type info
struct Type {
    template<typename T> static const Type& reflect();

public:
    Type() = default;
    Type(const Type&);
    Type(Type&&) noexcept;
    Type(EType);
    ~Type();
    Type& operator=(const Type);
    Type& operator=(Type&&) noexcept;

public:
    /// @note [0] is main type
    /// e.g.
    /// [0] DEQUE [1] INT == Deque<int> 
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

/// @brief container of class fiedls and enum values list reflector
/// @note  Relfector<MyClass> == MyClass reflector class
/// @tparam T class or enum
template<typename T> struct Reflector {
    /// @tparam C constructor: type info to create
    template<class C> static const Reflector<T>& reflect();

public:
    template<typename C> static const Reflector<T>& find();              //!< get registred C type data
    template<typename C> static const Reflector<T>& find(const C&);      //!< get registred C type data
    static const Reflector<T>&                      find(const char*);   //!< get registred C type data by name c string
    static const Reflector<T>&                      find(const string&); //!< get registred C type data by name string

public:
    Reflector() = default;
    Reflector(const Reflector&);
    Reflector(Reflector&&) noexcept;
    Reflector(const std::initializer_list<T>&);
    ~Reflector();

public:
    Reflector& operator=(const Reflector);
    Reflector& operator=(Reflector&&) noexcept;

public:
    const T& operator[](size_t) const;

public:
    const T* begin() const;
    const T* end() const;
    size_t   size() const;

private:
    template<typename Arg> void push(Arg&&);
    void                        shrink();

private:
    T*     data      = nullptr;
    size_t capacitor = 0;
    size_t count     = 0;

private:
    inline static std::unordered_map<string, Reflector<T>> map;
};

/// @brief variable info
struct Variable {
    Type        type;
    const char* name;
    size_t      size;
};

/// @brief member variable info
struct Field: Variable {
    size_t offset;
};

/// @brief enum info
struct Enumerator {
    uint64      value;
    const char* name;
};

using Enumerate = Reflector<Enumerator>;
using Structure = Reflector<Field>;

/// @brief class metadata
struct Class {
    virtual const char*      name() const    = 0;
    virtual size_t           size() const    = 0;
    virtual const Structure& fields() const  = 0;
    virtual const Class*     base() const    = 0;
    virtual const Object*    statics() const = 0;
};

/// @brief enum metadata
struct Enum {
    virtual const char*      name() const  = 0;
    virtual size_t           size() const  = 0;
    virtual const Enumerate& enums() const = 0;

public:
    template<typename E> static const char* serialize(E);
    static const char*                      serialize(const string&, uint64);
    static const char*                      serialize(const char*, uint64);

public:
    template<typename E> static E deserialize(const char*);
    template<typename E> static E deserialize(const string&);
    static uint64_t               deserialize(const string&, const string&);
};

/// @brief static object registry
/// @note  Registry<Class> == metadata registry
/// @tparam T ONLY Class(class metadata) or Object(static instance)
template<typename T> class Registry {
public:
    /// @tparam U base of T
    template<typename U> static void add(const string&);
    /// @tparam U base of T
    template<typename U> static void add(const char*);

public:
    static T* find(const char*);
    static T* find(const string&);

private:
    Registry() = default;

public:
    ~Registry();

private:
    std::unordered_map<string, T*> map;

private:
    static std::unordered_map<string, T*>* instance();
};

/// unused type
enum class Registered : bool {
    REGISTERED = 1
};

/// @brief  pre-registered metadata of typename T, return value is unused
template<typename T> Registered registclass();
/// @brief  pre-registered metadata of typename T, return value is unused
template<typename T> Registered registenum();

template<typename T> constexpr EType typecode(); //!< get type code

constexpr const char*            typestring(EType);       //!< reflect type name string by enum
template<typename T> const char* typestring();            //!< reflect type name string explicit
template<typename T> const char* typestring(const T&);    //!< reflect type name string implicit
const char*                      typestring(const Type&); //!< reflect type name

template<typename T> const Type& typeof();         //!< reflect typeinfo by template
template<typename T> const Type& typeof(const T&); //!< reflect typeinfo by argument
template<typename T> void        typeof(Type*);    //!< pirvate

template<typename T> Object* statics();              //!< get static class
template<typename T> Object* statics(const T&);      //!< get static class
Object*                      statics(const char*);   //!< get static class
Object*                      statics(const string&); //!< get static class

template<typename T> Class* classof();              //!< get class field list
template<typename T> Class* classof(const T&);      //!< get class field list
Class*                      classof(const char*);   //!< get class field list
Class*                      classof(const string&); //!< get class field list

template<typename T> Enum* enumof();              //!< get enum value list
template<typename T> Enum* enumof(const T&);      //!< get enum value list
Enum*                      enumof(const char*);   //!< get enum value list
Enum*                      enumof(const string&); //!< get enum value list

} // namespace meta
LWE_END

template<> struct std::hash<LWE::meta::Type> {
    size_t operator()(const LWE::meta::Type& obj) const { return obj.hash(); }
};
#endif