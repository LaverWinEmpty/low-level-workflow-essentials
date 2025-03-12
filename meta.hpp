#ifndef LWE_META_HEADER
#define LWE_META_HEADER

#include "enum.hpp"
#include "container.hpp"

/**************************************************************************************************
 * Meta~
 * - MetaType      | type name to enum
 * - MetaAccess    | access modifier to enum
 * - MetaField     | field information struct
 * - MetaMethod    | method information struct
 * - MetaClass     | class information struct
 * - MetaContainer | container information getter struct
 *
 * functions
 * - typecode   | get type enum
 * - typeinfo   | get type information vector
 * - typestring | get type name string
 **************************************************************************************************/

// clang-format off

enum class MetaType : int8 {
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
    CLASS,
    UNION,
    POINTER,
    REFERENCE,
    FUNCTION,
    STD_STRING,
    STL_DEQUE,
};

enum class MetaAccess : int8 {
    PRIVATE,
    PROTECTED,
    PUBLIC,
    NONE,
};

// get container type code structur
template<typename, typename = std::void_t<>> struct MetaContainer {
    static constexpr MetaType CODE = MetaType::UNREGISTERED;
};

/**
 * @brief type code: primitive type has 1 element, but pointer, reference, template, etc has more elements
 */
struct TypeInfo {
private:
    static constexpr size_t STACK = (sizeof(size_t) + sizeof(MetaType*));
    void                    push(MetaType in);

public:
    template<typename T> static void make(TypeInfo* out);

public:
    TypeInfo() = default;
    TypeInfo(const TypeInfo&);
    TypeInfo(TypeInfo&&) noexcept;
    ~TypeInfo();
    TypeInfo& operator=(const TypeInfo);
    TypeInfo& operator=(TypeInfo&&) noexcept;
    
public:
    const MetaType& operator[](size_t) const;
    const MetaType* begin() const;
    const MetaType* end() const;
    size_t          size() const;
    hash_t          hash() const;
    const char*     c_str() const;

public:
    explicit operator string() const;
    operator MetaType() const;

private:
    size_t count = 0;
    union {
        struct {
            MetaType* heap;
            size_t    capacitor;
        };
        MetaType stack[STACK] = { MetaType::UNREGISTERED };
    };
};

template<> struct std::hash<TypeInfo> {
    size_t operator()(const TypeInfo& obj) const {
        return obj.hash();
    } 
};

template<typename T> constexpr MetaType typecode();                     //!< get type enum value
template<> constexpr           MetaType typecode<void>();               //!< get type enum value
template<> constexpr           MetaType typecode<signed int>();         //!< get type enum value
template<> constexpr           MetaType typecode<signed char>();        //!< get type enum value
template<> constexpr           MetaType typecode<signed short>();       //!< get type enum value
template<> constexpr           MetaType typecode<signed long>();        //!< get type enum value
template<> constexpr           MetaType typecode<signed long long>();   //!< get type enum value
template<> constexpr           MetaType typecode<unsigned int>();       //!< get type enum value
template<> constexpr           MetaType typecode<unsigned char>();      //!< get type enum value
template<> constexpr           MetaType typecode<unsigned short>();     //!< get type enum value
template<> constexpr           MetaType typecode<unsigned long>();      //!< get type enum value
template<> constexpr           MetaType typecode<unsigned long long>(); //!< get type enum value
template<> constexpr           MetaType typecode<bool>();               //!< get type enum value
template<> constexpr           MetaType typecode<char>();               //!< get type enum value
template<> constexpr           MetaType typecode<wchar_t>();            //!< get type enum value
template<> constexpr           MetaType typecode<float>();              //!< get type enum value
template<> constexpr           MetaType typecode<double>();             //!< get type enum value
template<> constexpr           MetaType typecode<long double>();        //!< get type enum value
template<> constexpr           MetaType typecode<string>();             //!< get type enum value

template<typename T> const TypeInfo& typeinfo();          //!< get typeinfo by template
template<typename T> const TypeInfo& typeinfo(const T&);  //!< get typeinfo by argument
template<typename T> void            typeinfo(TypeInfo*); //!< pirvate

/*
 * @breif metadata field
 */
struct MetaField {
    MetaAccess  level;  //!< NONE: exception
    TypeInfo    type;   //!< [0] is type: other is template parameters, [0] is pointer, reference count
    const char* name;   //!< vairable name
    size_t      size;   //!< variable size
    size_t      offset; //!< variable offset
};

// TODO:
/*
 * @breif metadata method
 */
struct MetaMethod {
    MetaAccess level;
    MetaType   result;
    TypeInfo   parameters;
};

/**
 * @brief member variable list
 */
using FieldInfo = std::vector<MetaField>;

/**
 * @brief member function list
 */
using MethodInfo = std::vector<MetaMethod>;

/*
 * @breif metadata class base
 */
struct MetaClass {
public:
    template<typename T> static MetaClass* get() {
        return nullptr;
    }
    
public:
    virtual const char*      name() const       = 0;
    virtual size_t           size() const       = 0;
    virtual const FieldInfo& field() const = 0;
    virtual MetaClass*       base() const       = 0;
};

constexpr const char*            typestring(MetaType);        //!< get type name string by enum
template<typename T> const char* typestring();                //!< get type name string explicit
template<typename T> const char* typestring(const T&);        //!< get type name string implicit
const char*                      typestring(const TypeInfo&); //!< get type name

constexpr bool                      isSTL(MetaType); //!< check container type code
template<typename T> constexpr bool isSTL();         //!< check container explicit
template<typename T> constexpr bool isSTL(const T&); //!< check container implicit

// clang-format on
#include "meta.ipp"
#endif