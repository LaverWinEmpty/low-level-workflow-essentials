// TODO: tsreal function
// need fix old version
// removed from this file

#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "std.hpp"
#include "macro.h"
#include "container.hpp"

using int8 = signed char;
using std::string;

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


REGISTER_CONTAINER(Deque, STL_DEQUE);

// get type
template<typename T> constexpr MetaType typecode() {
    if constexpr(std::is_base_of_v<LWE::stl::Container, T>) return MetaContainer<T>::CODE;
    if constexpr(std::is_pointer_v<T>)   return MetaType::POINTER;
    if constexpr(std::is_reference_v<T>) return MetaType::REFERENCE;
    if constexpr(std::is_union_v<T>)     return MetaType::UNION;
    if constexpr(std::is_class_v<T>)     return MetaType::CLASS; //!< TODO: UClass 처럼 조건 되는 Class만 Class로 변경
    return MetaType::UNREGISTERED;
}
template<> constexpr MetaType typecode<void>()               { return MetaType::VOID; }
template<> constexpr MetaType typecode<signed int>()         { return MetaType::SIGNED_INT; }
template<> constexpr MetaType typecode<signed char>()        { return MetaType::SIGNED_CHAR; }
template<> constexpr MetaType typecode<signed short>()       { return MetaType::SIGNED_SHORT; }
template<> constexpr MetaType typecode<signed long>()        { return MetaType::SIGNED_LONG; }
template<> constexpr MetaType typecode<signed long long>()   { return MetaType::SIGNED_LONG_LONG; }
template<> constexpr MetaType typecode<unsigned int>()       { return MetaType::UNSIGNED_INT; }
template<> constexpr MetaType typecode<unsigned char>()      { return MetaType::UNSIGNED_CHAR; }
template<> constexpr MetaType typecode<unsigned short>()     { return MetaType::UNSIGNED_SHORT; }
template<> constexpr MetaType typecode<unsigned long>()      { return MetaType::UNSIGNED_LONG; }
template<> constexpr MetaType typecode<unsigned long long>() { return MetaType::UNSIGNED_LONG_LONG; }
template<> constexpr MetaType typecode<bool>()               { return MetaType::BOOL; }
template<> constexpr MetaType typecode<char>()               { return MetaType::CHAR; }
template<> constexpr MetaType typecode<wchar_t>()            { return MetaType::WCHAR_T; }
template<> constexpr MetaType typecode<float>()              { return MetaType::FLOAT; }
template<> constexpr MetaType typecode<double>()             { return MetaType::DOUBLE; }
template<> constexpr MetaType typecode<long double>()        { return MetaType::LONG_DOUBLE; }
template<> constexpr MetaType typecode<string>()             { return MetaType::STD_STRING; }

using Types = std::vector<MetaType>;

// type serialize rec
template<typename T> void tserial(Types* out) {
    if constexpr(isSTL<T>()) {
        out->push_back(MetaContainer<T>::CODE);
        tserial<typename T::value_type>(out);
    }

    else {
        out->push_back(typecode<T>());
            if constexpr(std::is_pointer_v<T>) {
            tserial<typename std::remove_pointer_t<T>>(out);
        }
        else if(std::is_reference_v<T>) {
            tserial<typename std::remove_reference_t<T>>(out);
        }
    }
}

// type serialize
template<typename T> const Types& tserial() {
    static Types list;
    if (list.size() == 0) {
        tserial<T>(&list);
    }
    return list;
}

// type serialize
template<typename T> const Types& tserial(const T&) {
    return tserial<T>();
}

template<typename E> E evalue(size_t);   //!< declare index to enum
template<typename E> E evalue(string);   //!< declare string to enum
constexpr const char* estring(MetaType); //!< declare
constexpr const char* evalue(MetaType);  //!< declare

/*
 * @breif metadata field
 */
struct MetaField {
private:
    template<typename T> static void traits(std::vector<MetaType>* out) { out->push_back(typecode<T>()); }

public:
    template<typename T> static std::vector<MetaType> traits() {
        std::vector<MetaType> v;
        traits<T>(&v);
        return v;
    }

private:
    MetaAccess level;  //!< NONE: exception
    Types      type;   //!< [0] is type: other is template parameters, [0] is pointer, reference count
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
    Types      parameters;
};

/*
 * @breif metadata class base
 */
struct MetaClass {
public:
    template<typename T> static MetaClass* get() {
        static T vtable;
        return vtable.metaclass();
    }
public:
    virtual const char*  name() const       = 0;
    virtual size_t       size() const       = 0;
    virtual const Types& properties() const = 0;
    virtual MetaClass*   base() const       = 0;
};

template<typename T> constexpr bool isSTL() {
    return std::is_base_of_v<LWE::stl::Container, T>;
};

constexpr bool isSTL(MetaType code) {
    const char* name = estring(code);
    if (name[0] == 'S' && name[1] == 'T' && name[2] == 'L' && name[3] == '_') {
        return true;
    }
    return false;
}

constexpr const char* typestring(MetaType code) {
    switch(code) {
    case MetaType::UNREGISTERED:       return "";
    case MetaType::VOID:               return "void";
    case MetaType::SIGNED_INT:         return "int";
    case MetaType::SIGNED_CHAR:        return "char";
    case MetaType::SIGNED_SHORT:       return "short";
    case MetaType::SIGNED_LONG:        return "long";
    case MetaType::SIGNED_LONG_LONG:   return "long long";
    case MetaType::UNSIGNED_SHORT:     return "unsigned short";
    case MetaType::UNSIGNED_INT:       return "unsigned int";
    case MetaType::UNSIGNED_CHAR:      return "unsigned char";
    case MetaType::UNSIGNED_LONG:      return "unsigned long";
    case MetaType::UNSIGNED_LONG_LONG: return "unsigned long long";
    case MetaType::BOOL:               return "bool";
    case MetaType::CHAR:               return "char";
    case MetaType::WCHAR_T:            return "wchar_t";
    case MetaType::FLOAT:              return "float";
    case MetaType::DOUBLE:             return "double";
    case MetaType::LONG_DOUBLE:        return "long double";
    case MetaType::CLASS:              return "class";
    case MetaType::UNION:              return "union";
    case MetaType::POINTER:            return "*";
    case MetaType::REFERENCE:          return "&";
    case MetaType::FUNCTION:           return "function";
    case MetaType::STD_STRING:         return "string";
    case MetaType::STL_DEQUE:          return "Deque";
    }

    // error
    return "";
}

constexpr size_t typestring(string* out, const Types& in, size_t idx) {
    // pointer or reference
    if (in[idx] == MetaType::POINTER || in[idx] == MetaType::REFERENCE) {
        typestring(out, in, idx + 1); // rec
    }

    out->append(typestring(in[idx]));

    // has template
    if (isSTL(in[idx])) {
        out->append("<");
        typestring(out, in, idx + 1);
        
        // TODO: map 처리

        out->append(">");
    }

    return idx + 1;
}

// runtime type name string
string typestring(const Types& in) {
    string out;
    typestring(&out, in, 0);
    return out;
}

template<typename T> const char* typestring() {
    static string str = typestring(tserial<T>());
    return str.c_str();
}

template<typename T> const char* typestring(const T&) {
    static string str = typestring(tserial<T>());
    return str.c_str();
}

constexpr int8 typevalue(MetaType code) {
    return static_cast<int8>(code); 
}

template<typename E> size_t emax(E = static_cast<E>(0)) {
    return static_cast<size_t>(eval<E>(-1));
}

// register estring type code
REGISTER_ENUM_TO_STRING_BEGIN(MetaType) {
    REGISTER_ENUM_TO_STRING(UNREGISTERED);
    REGISTER_ENUM_TO_STRING(VOID);
    REGISTER_ENUM_TO_STRING(SIGNED_INT);
    REGISTER_ENUM_TO_STRING(SIGNED_CHAR);
    REGISTER_ENUM_TO_STRING(SIGNED_SHORT);
    REGISTER_ENUM_TO_STRING(SIGNED_LONG);
    REGISTER_ENUM_TO_STRING(SIGNED_LONG_LONG);
    REGISTER_ENUM_TO_STRING(UNSIGNED_SHORT);
    REGISTER_ENUM_TO_STRING(UNSIGNED_INT);
    REGISTER_ENUM_TO_STRING(UNSIGNED_CHAR);
    REGISTER_ENUM_TO_STRING(UNSIGNED_LONG);
    REGISTER_ENUM_TO_STRING(UNSIGNED_LONG_LONG);
    REGISTER_ENUM_TO_STRING(BOOL);
    REGISTER_ENUM_TO_STRING(CHAR);
    REGISTER_ENUM_TO_STRING(WCHAR_T);
    REGISTER_ENUM_TO_STRING(FLOAT);
    REGISTER_ENUM_TO_STRING(DOUBLE);
    REGISTER_ENUM_TO_STRING(LONG_DOUBLE);
    REGISTER_ENUM_TO_STRING(CLASS);
    REGISTER_ENUM_TO_STRING(UNION);
    REGISTER_ENUM_TO_STRING(POINTER);
    REGISTER_ENUM_TO_STRING(REFERENCE);
    REGISTER_ENUM_TO_STRING(FUNCTION);
    REGISTER_ENUM_TO_STRING(STD_STRING);
    REGISTER_ENUM_TO_STRING(STL_DEQUE);
} REGISTER_ENUM_TO_STRING_END;

// register eval type code
REGISTER_STRING_TO_ENUM_BEGIN(MetaType) {
    REGISTER_STRING_TO_ENUM(UNREGISTERED);
    REGISTER_STRING_TO_ENUM(VOID);
    REGISTER_STRING_TO_ENUM(SIGNED_INT);
    REGISTER_STRING_TO_ENUM(SIGNED_CHAR);
    REGISTER_STRING_TO_ENUM(SIGNED_SHORT);
    REGISTER_STRING_TO_ENUM(SIGNED_LONG);
    REGISTER_STRING_TO_ENUM(SIGNED_LONG_LONG);
    REGISTER_STRING_TO_ENUM(UNSIGNED_SHORT);
    REGISTER_STRING_TO_ENUM(UNSIGNED_INT);
    REGISTER_STRING_TO_ENUM(UNSIGNED_CHAR);
    REGISTER_STRING_TO_ENUM(UNSIGNED_LONG);
    REGISTER_STRING_TO_ENUM(UNSIGNED_LONG_LONG);
    REGISTER_STRING_TO_ENUM(BOOL);
    REGISTER_STRING_TO_ENUM(CHAR);
    REGISTER_STRING_TO_ENUM(WCHAR_T);
    REGISTER_STRING_TO_ENUM(FLOAT);
    REGISTER_STRING_TO_ENUM(DOUBLE);
    REGISTER_STRING_TO_ENUM(LONG_DOUBLE);
    REGISTER_STRING_TO_ENUM(CLASS);
    REGISTER_STRING_TO_ENUM(UNION);
    REGISTER_STRING_TO_ENUM(POINTER);
    REGISTER_STRING_TO_ENUM(REFERENCE);
    REGISTER_STRING_TO_ENUM(FUNCTION);
    REGISTER_STRING_TO_ENUM(STD_STRING);
    REGISTER_STRING_TO_ENUM(STL_DEQUE);
} REGISTER_STRING_TO_ENUM_END;

// register estring access modifier
REGISTER_ENUM_TO_STRING_BEGIN(MetaAccess) {
    REGISTER_ENUM_TO_STRING(NONE);
    REGISTER_ENUM_TO_STRING(PRIVATE);
    REGISTER_ENUM_TO_STRING(PROTECTED);
    REGISTER_ENUM_TO_STRING(PUBLIC);
} REGISTER_ENUM_TO_STRING_END;

// register eval access modifier
REGISTER_STRING_TO_ENUM_BEGIN(MetaAccess) {
    REGISTER_STRING_TO_ENUM(NONE);
    REGISTER_STRING_TO_ENUM(PRIVATE);
    REGISTER_STRING_TO_ENUM(PROTECTED);
    REGISTER_STRING_TO_ENUM(PUBLIC);
} REGISTER_STRING_TO_ENUM_END;

// clang-format on