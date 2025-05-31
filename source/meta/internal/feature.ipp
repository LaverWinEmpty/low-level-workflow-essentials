#ifdef LWE_META_FEATURE

#include "reflector.hpp"

LWE_BEGIN
namespace meta {

// clang-format off
template<typename T> constexpr Keyword typecode() {
    if constexpr(std::is_base_of_v<LWE::meta::Container, T>) return ContainerCode<T>::VALUE;
    if constexpr(std::is_enum_v<T>)                          return Keyword::ENUM;
    if constexpr(std::is_pointer_v<T>)                       return Keyword::POINTER;
    if constexpr(std::is_reference_v<T>)                     return Keyword::REFERENCE;
    if constexpr(std::is_union_v<T>)                         return Keyword::UNION;
    if constexpr(std::is_class_v<T>)                         return Keyword::CLASS;
    if constexpr(std::is_void_v<T>)                          return Keyword::VOID;
    if constexpr(std::is_same_v<T, bool>)                    return Keyword::BOOL;
    if constexpr(std::is_same_v<T, signed char>)             return Keyword::SIGNED_CHAR;
    if constexpr(std::is_same_v<T, unsigned char>)           return Keyword::UNSIGNED_CHAR;
    if constexpr(std::is_same_v<T, char>)                    return Keyword::CHAR;
    if constexpr(std::is_same_v<T, signed short>)            return Keyword::SIGNED_SHORT;
    if constexpr(std::is_same_v<T, unsigned short>)          return Keyword::UNSIGNED_SHORT;
    if constexpr(std::is_same_v<T, signed int>)              return Keyword::SIGNED_INT;
    if constexpr(std::is_same_v<T, unsigned int>)            return Keyword::UNSIGNED_INT;
    if constexpr(std::is_same_v<T, signed long>)             return Keyword::SIGNED_LONG;
    if constexpr(std::is_same_v<T, unsigned long>)           return Keyword::UNSIGNED_LONG;
    if constexpr(std::is_same_v<T, wchar_t>)                 return Keyword::WCHAR_T;
    if constexpr(std::is_same_v<T, float>)                   return Keyword::FLOAT;
    if constexpr(std::is_same_v<T, double>)                  return Keyword::DOUBLE;
    if constexpr(std::is_same_v<T, long double>)             return Keyword::LONG_DOUBLE;
    if constexpr(std::is_same_v<T, std::string>)             return Keyword::STD_STRING;
    else                                                     return Keyword::UNREGISTERED;
}
// clang-format on

// clang-format off
constexpr const char* typestring(Keyword code) {
    switch(code) {
    case Keyword::UNREGISTERED:       return "";
    case Keyword::VOID:               return "void";
    case Keyword::SIGNED_INT:         return "int";
    case Keyword::SIGNED_CHAR:        return "char";
    case Keyword::SIGNED_SHORT:       return "short";
    case Keyword::SIGNED_LONG:        return "long";
    case Keyword::SIGNED_LONG_LONG:   return "long long";
    case Keyword::UNSIGNED_SHORT:     return "unsigned short";
    case Keyword::UNSIGNED_INT:       return "unsigned int";
    case Keyword::UNSIGNED_CHAR:      return "unsigned char";
    case Keyword::UNSIGNED_LONG:      return "unsigned long";
    case Keyword::UNSIGNED_LONG_LONG: return "unsigned long long";
    case Keyword::BOOL:               return "bool";
    case Keyword::CHAR:               return "char";
    case Keyword::WCHAR_T:            return "wchar_t";
    case Keyword::FLOAT:              return "float";
    case Keyword::DOUBLE:             return "double";
    case Keyword::LONG_DOUBLE:        return "long double";
    case Keyword::CLASS:              return "class";
    case Keyword::UNION:              return "union";
    case Keyword::POINTER:            return "*";
    case Keyword::REFERENCE:          return "&";
    case Keyword::FUNCTION:           return "function";
    case Keyword::STD_STRING:         return "string";
    case Keyword::STL_DEQUE:          return "Deque";
    case Keyword::CONST:              return "const";
    case Keyword::ENUM:               return "enum";
    }
    return ""; // error
}
// clang-format on

//template<typename T> Registered registclass() {
//    static_assert(std::is_same_v<T, Object>);
//
//    // default, other class -> template specialization
//    Structure::reflect<Object>();
//    Registry<Object>::add<Object>("Object");
//    Registry<Class>::add<Class>("Object");
//    return Registered::REGISTERED;
//}
//
//template<typename T> Registered registenum() {
//    // default, other enum -> template specialization
//    return Registered::REGISTERED;
//}

// clang-format off

template<typename T> constexpr bool isSTL() {
    return std::is_base_of_v<LWE::meta::Container, T> && ContainerCode<T>::VALUE != Keyword::UNREGISTERED;
};

template<typename T> constexpr bool isSTL(const T&) {
    return isSTL<T>();
}

template<> bool isSTL<Keyword>(const Keyword& code) {
    switch(code) {
        case Keyword::STL_DEQUE: return true;
    }
    return false;
}

}
LWE_END
#endif