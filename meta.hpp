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

public:
    template<typename T> static void make(TypeInfo* out) {
        if constexpr(isSTL<T>()) {
            out->push(MetaContainer<T>::CODE);
            make<typename T::value_type>(out);
        } else {
            out->push(typecode<T>());
            if constexpr(std::is_pointer_v<T>) {
                make<typename std::remove_pointer_t<T>>(out); // dereference
            } else if constexpr(std::is_reference_v<T>) {
                make<typename std::remove_reference_t<T>>(out); // dereference
            }
        }
    }

public:
    ~TypeInfo() {
        if(count >= STACK) {
            free(heap);
        }
    }

public:
    const MetaType& operator[](size_t idx) const {
        if(count < STACK) {
            return stack[idx];
        } else return heap[idx];
    }

public:
    operator MetaType() const {
        if(count < STACK) {
            return *stack;
        }
        return *heap;
    }

private:
    void push(MetaType in) {
        size_t next = count + 1;

        // swap
        if(next == STACK) {
            MetaType buffer[STACK];
            std::memcpy(buffer, stack, STACK);

            capacitor = STACK << 1; // multiple of 2
            heap      = static_cast<MetaType*>(malloc(sizeof(MetaType) * capacitor));
            if(!heap) {
                std::memcpy(stack, buffer, STACK); // rollback
                throw std::bad_alloc();            // failed
            }
            std::memcpy(heap, buffer, STACK);

            heap[count] = in;
            count       = next;
        }

        // use heap
        else if(next > STACK) {
            // reallocate
            if(next >= capacitor) {
                capacitor       <<= 1;
                MetaType* newly   = static_cast<MetaType*>(realloc(stack, capacitor));
                if(!newly) {
                    capacitor >>= 1;
                    throw std::bad_alloc();
                }
                heap = newly;
            }

            heap[count] = in;
            count       = next;
        }

        // use stack
        else {
            stack[count] = in;
            count        = next;
        }
    }

public:
    const size_t& size = count; // read only

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
private:
    template<typename T> static void traits(TypeInfo* out) { out->push(typecode<T>()); }

public:
    template<typename T> static TypeInfo traits() {
        static TypeInfo v;
        if(v.count() == 0) {
            traits<T>(&v);
        }
        return v;
    }

public:
    MetaAccess level;  //!< NONE: exception
    TypeInfo   type;   //!< [0] is type: other is template parameters, [0] is pointer, reference count
    string     name;   //!< value name
    size_t     size;   //!< size
    size_t     offset; //!< filed offset
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
    virtual const FieldInfo& properties() const = 0;
    virtual MetaClass*       base() const       = 0;
};

string                           typestring(const TypeInfo&); //!< get type name string
constexpr const char*            typestring(MetaType);        //!< get type name string by enum
template<typename T> const char* typestring();                //!< get type name string explicit
template<typename T> const char* typestring(const T&);        //!< get type name string implicit

constexpr bool                      isSTL(MetaType); //!< check container type code
template<typename T> constexpr bool isSTL();         //!< check container explicit
template<typename T> constexpr bool isSTL(const T&); //!< check container implicit

// clang-format on
#include "meta.ipp"
#endif