#ifndef LWE_META_HEADER
#define LWE_META_HEADER

#include "hal.hpp"
#include "common.hpp"

std::unordered_map<string, std::vector<std::pair<string, uint64>>> enumerate;

enum class EType : uintptr_t {
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

// get container type code structur
template<typename, typename = std::void_t<>> struct ContainerCode {
    static constexpr EType VALUE = EType::UNREGISTERED;
};

/**
 * @brief
 *
 */
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
    const EType& operator[](size_t) const;
    const EType* begin() const;
    const EType* end() const;
    size_t       size() const;
    hash_t       hash() const;
    EType        type() const;
    const char*  stringify() const;

public:
    explicit operator string() const;
    operator EType() const;

private:
    static constexpr size_t STACK = (sizeof(size_t) + sizeof(EType*));

private:
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

// clang-format off
template<typename T> constexpr EType typecode();                     //!< reflect type enum value

constexpr const char*            typestring(EType);      //!< reflect type name string by enum
template<typename T> const char* typestring();            //!< reflect type name string explicit
template<typename T> const char* typestring(const T&);    //!< reflect type name string implicit
const char*                      typestring(const Type&); //!< reflect type name

template<typename T> const Type& typeof();          //!< reflect typeinfo by template
template<typename T> const Type& typeof(const T&);  //!< reflect typeinfo by argument
template<typename T> void        typeof(Type*);     //!< pirvate
// clang-format on

struct Variable {
    Type        type;
    const char* name;
    size_t      size;
};

struct Field: Variable {
    size_t offset;
};

struct Enumerator {
    uint64      value;
    const char* name;
};

template<typename T> struct Reflect {
    template<class C> static const Reflect<T>& reflect();

public:
    template<typename C> static const Reflect<T>& get();
    template<typename C> static const Reflect<T>& get(const C&);
    static const Reflect<T>&                      get(const char*);

public:
    template<typename C> bool flag() const;
    template<typename C> bool flag(const C&) const;
    bool                      flag(const char*) const;

public:
    Reflect() = default;
    Reflect(const Reflect&);
    Reflect(Reflect&&) noexcept;
    Reflect(const std::initializer_list<T>&);
    ~Reflect();

public:
    Reflect& operator=(const Reflect);
    Reflect& operator=(Reflect&&) noexcept;

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
    inline static std::unordered_map<const char*, Reflect<T>> map;
};

using Enumerate = Reflect<Enumerator>;
using Structure = Reflect<Field>;

enum class Registered : bool {
    REGISTERED = 1
};

struct Class {
    friend class Object;
    template<typename T> friend Registered registclass();
    friend Class*                          metaclass(const string&);

public:
    virtual const char*      name() const   = 0;
    virtual size_t           size() const   = 0;
    virtual const Structure& fields() const = 0;
    virtual const Class*     base() const   = 0;
};

struct Enum {
    template<typename T> friend Registered registenum();
    friend Enum*                           metaenum(const string&);

public:
    virtual const char*      name() const  = 0;
    virtual size_t           size() const  = 0;
    virtual const Enumerate& enums() const = 0;
};

template<typename T> class Register {
public:
    using Map = std::unordered_map<string, T*>;

public:
    template<typename U> static void set(const string&);
    template<typename U> static void set(const char*);

public:
    static T* get(const char*);
    static T* get(const string&);

private:
    Register() = default;
    ~Register();

private:
    Map map;

private:
    static Map& registry();
};

template<typename T> constexpr bool isSTL();                    //!< check container explicit
template<typename T> constexpr bool isSTL(const T&);            //!< check container implicit
template<> bool                     isSTL<EType>(const EType&); //!< check container type code

template<typename T> Registered registclass();
template<typename T> Registered registenum();

template<typename T> const Object* statics();
template<typename T> const Object* statics(const T&);
const Object*                      statics(const char*);
const Object*                      statics(const string&);

template<typename T> Class* metaclass();
template<typename T> Class* metaclass(const T&);
Class*                      metaclass(const char*);
Class*                      metaclass(const string&);

template<typename T> Enum* metaenum();
template<typename T> Enum* metaenum(const T&);
Enum*                      metaenum(const char*);
Enum*                      metaenum(const string&);

template<> struct std::hash<Type> {
    size_t operator()(const Type& obj) const { return obj.hash(); }
};

#endif
