#ifndef LWE_META_HEADER
#define LWE_META_HEADER

#include "hal.hpp"
#include "common.hpp"

/**************************************************************************************************
 * Meta~
 * - type_t      | type name to enum
 * - access_t    | access modifier to enum
 * - MetaField     | fields information struct
 * - MetaMethod    | method information struct
 * - MetaClass     | class information struct
 * - MetaContainer | container information getter struct
 *
 * functions
 * - typecode   | get type enum
 * - typeinfo   | get type information vector
 * - typestring | get type name string
 **************************************************************************************************/

enum class EType : int8 {
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
 * @brief type code: primitive type has 1 element, but pointer, reference, template, etc has more elements
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

struct EInterface {
    virtual std::string serialize() const               = 0;
    virtual void        deserialize(const std::string&) = 0;
};

// clang-format off

template<typename T> constexpr EType typecode();                     //!< reflect type enum value
template<> constexpr           EType typecode<void>();               //!< reflect type enum value
template<> constexpr           EType typecode<signed int>();         //!< reflect type enum value
template<> constexpr           EType typecode<signed char>();        //!< reflect type enum value
template<> constexpr           EType typecode<signed short>();       //!< reflect type enum value
template<> constexpr           EType typecode<signed long>();        //!< reflect type enum value
template<> constexpr           EType typecode<signed long long>();   //!< reflect type enum value
template<> constexpr           EType typecode<unsigned int>();       //!< reflect type enum value
template<> constexpr           EType typecode<unsigned char>();      //!< reflect type enum value
template<> constexpr           EType typecode<unsigned short>();     //!< reflect type enum value
template<> constexpr           EType typecode<unsigned long>();      //!< reflect type enum value
template<> constexpr           EType typecode<unsigned long long>(); //!< reflect type enum value
template<> constexpr           EType typecode<bool>();               //!< reflect type enum value
template<> constexpr           EType typecode<char>();               //!< reflect type enum value
template<> constexpr           EType typecode<wchar_t>();            //!< reflect type enum value
template<> constexpr           EType typecode<float>();              //!< reflect type enum value
template<> constexpr           EType typecode<double>();             //!< reflect type enum value
template<> constexpr           EType typecode<long double>();        //!< reflect type enum value
template<> constexpr           EType typecode<string>();             //!< reflect type enum value

constexpr const char*            typestring(EType);      //!< reflect type name string by enum
template<typename T> const char* typestring();            //!< reflect type name string explicit
template<typename T> const char* typestring(const T&);    //!< reflect type name string implicit
const char*                      typestring(const Type&); //!< reflect type name

template<typename T> const Type& typeof();          //!< reflect typeinfo by template
template<typename T> const Type& typeof(const T&);  //!< reflect typeinfo by argument
template<typename T> void        typeof(Type*);     //!< pirvate
// clang-format on

/*
 * @breif metadata fields
 */
struct Variable {
    Type        type;
    const char* name;
    size_t      size;
    size_t      offset;
};

struct Structure {
public:
    template<typename T> static const Structure& reflect(); // define by macro

public:
    Structure() = default;
    Structure(const Structure&);
    Structure(Structure&&) noexcept;
    Structure(const std::initializer_list<Variable>&);
    ~Structure();

public:
    Structure& operator=(const Structure);
    Structure& operator=(Structure&&) noexcept;

public:
    const Variable& operator[](size_t) const;

public:
    const Variable* begin() const;
    const Variable* end() const;
    size_t          size() const;

private:
    template<typename Arg> void push(Arg&&); //!< USE ON REFLECT ONLY

private:
    Variable* fields    = nullptr;
    size_t    capacitor = 0;
    size_t    count     = 0;
};

// template<> const Structure::Fields& Structure::reflect<>();

/*
 * @breif metadata class base
 */
struct MetaClass {
public:
    template<typename T> static MetaClass* get();
    template<typename T> static MetaClass* get(const T&);
public:
    virtual const char*      name() const   = 0;
    virtual size_t           size() const   = 0;
    virtual const Structure& fields() const = 0;
    virtual MetaClass*       base() const   = 0;
};

template<typename T> constexpr bool isSTL();                    //!< check container explicit
template<typename T> constexpr bool isSTL(const T&);            //!< check container implicit
template<> bool                     isSTL<EType>(const EType&); //!< check container type code

template<typename T> constexpr bool isEnum();                    //!< check enum explicit
template<typename T> constexpr bool isEnum(const T&);            //!< check enum implicit
template<> bool                     isEnum<EType>(const EType&); //!< check enum type code

template<typename E> E evalue(size_t);        //!< declare index to enum for template specialization
template<typename E> E evalue(const string&); //!< declare string to enum for template specialization

// reflect enum max
template<typename E> size_t emax(E) {
    return static_cast<size_t>(eval<E>(-1));
}

template<> struct std::hash<Type> {
    size_t operator()(const Type& obj) const { return obj.hash(); }
};

#endif