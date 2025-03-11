#include "meta.hpp"
#ifdef LWE_META_HEADER
// clang-format off

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


// get type enum
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

template<typename T> void typeinfo(TypeInfo* out) {
    if constexpr(isSTL<T>()) {
        out->push_back(MetaContainer<T>::CODE);
        typeinfo<typename T::value_type>(out);
    }
    else {
        out->push_back(typecode<T>());
        if constexpr(std::is_pointer_v<T>) {
            typeinfo<typename std::remove_pointer_t<T>>(out); // dereference
        }
        else if constexpr(std::is_reference_v<T>) {
            typeinfo<typename std::remove_reference_t<T>>(out); // dereference
        }
    }
}

template<typename T> const TypeInfo& typeinfo() {
    static TypeInfo list;
    if (list.size() == 0) {
        typeinfo<T>(&list);
    }
    return list;
}

template<typename T> const TypeInfo& typeinfo(const T&) {
    return typeinfo<T>();
}

constexpr bool isSTL(MetaType code) {
    const char* name = estring(code);
    if (name[0] == 'S' && name[1] == 'T' && name[2] == 'L' && name[3] == '_') {
        return true; // read 4 byte
    }
    return false;
}

template<typename T> constexpr bool isSTL() {
    return std::is_base_of_v<LWE::stl::Container, T> && MetaContainer<T>::CODE != MetaType::UNREGISTERED;
};

template<typename T> constexpr bool isSTL(const T&) {
    return isSTL<T>();
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

constexpr size_t typestring(string* out, const TypeInfo& in, size_t idx) {
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

string typestring(const TypeInfo& in) {
    std::function<size_t(string*, const TypeInfo&, size_t)> fn = [&fn](string* out, const TypeInfo& in, size_t idx) {
        // pointer or reference
        if(in[idx] == MetaType::POINTER || in[idx] == MetaType::REFERENCE) {
            fn(out, in, idx + 1); // rec
        }

        out->append(typestring(in[idx]));

        // has template
        if(isSTL(in[idx])) {
            out->append("<");
            fn(out, in, idx + 1);

            // TODO: map 처리

            out->append(">");
        }

        return idx + 1;
    };

    string out;
    fn(&out, in, 0);
    return out;
}

// call by template
template<typename T> const char* typestring() {
    static string str = typestring(typeinfo<T>());
    return str.c_str();
}

// call by argument
template<typename T> const char* typestring(const T&) {
    static string str = typestring(typeinfo<T>());
    return str.c_str();
}

// clang-format on
#endif