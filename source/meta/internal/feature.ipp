#ifdef LWE_META_FEATURE

#include "class.hpp"
#include "enum.hpp"
#include "method.hpp"

#include "../../util/hash.hpp"

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

// call by template
template<typename T> const char* typestring() {
    Type statics = typeof<T>();
    return statics.serialize();
}

// call by argument
template<typename T> const char* typestring(const T&) {
    return typestring<T>();
}

const char* typestring(const Type& in) {
    return *in;
}

template<typename T> const Type& typeof() {
    return Type::reflect<T>();
}

template<typename T> const Type& typeof(const T&) {
    return Type::reflect<T>();
}

template<typename T> Registered registclass() {
    // default, other class -> template specialization
    Structure::reflect<Object>();
    Registry<Object>::add<Object>("Object");
    Registry<Class>::add<Class>("Object");
    return Registered::REGISTERED;
}

template<typename T> Registered registenum() {
    // default, other enum -> template specialization
    return Registered::REGISTERED;
}

template<typename T> Object* statics() {
    // default, other class -> specialization
    if constexpr(!std::is_same_v<T, Object>) {
        return nullptr;
    }
    static Object* statics = Registry<Object>::find("Object");
    return statics;
}

template<typename T> Object* statics(const T&) {
    return statics<T>();
}

Object* statics(const char* in) {
    return statics(string{ in });
}

Object* statics(const string& in) {
    return Registry<Object>::find(in);
}

template<typename T> Class* classof() {
    // default, other class -> template specialization
    if constexpr(!std::is_same_v<T, Object>) {
        return nullptr;
    }

    static Class* meta = Registry<Class>::find("Object");
    return meta;
}

template<typename T> Class* classof(const T&) {
    return classof<T>();
}

Class* classof(const char* in) {
    return classof(string{ in });
}

Class* classof(const string& in) {
    return Registry<Class>::find(in);
}

template<typename T> Enum* enumof() {
    return nullptr;
}

template<typename T> Enum* enumof(const T&) {
    return enumof<T>();
}

Enum* enumof(const char* in) {
    return enumof(string{ in });
}

Enum* enumof(const string& in) {
    return Registry<Enum>::find(in);
}

template<typename T> Method* method(const char* name) {
    return method<T>(string{ name });
}

template<typename T> Method* method(const string& name) {
    // default, other class -> template specialization
    return nullptr;
}

Method* method(const char* cls, const char* name) {
    return Registry<Method>::find(cls, name);
}

Method* method(const string& cls, const char* name) {
    return Registry<Method>::find(cls, name);
}

Method* method(const char* cls, const string& name) {
    return Registry<Method>::find(cls, name);
}

Method* method(const string& cls, const string& name) {
    return Registry<Method>::find(cls, name);
}

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

/*
 * serialize
 */

// clang-format off

// primitive type to string
template<typename T> string serialize(const T& in) {
    std::stringstream ss;
    if constexpr(std::is_same_v<T, float>)       ss << std::fixed << std::setprecision(6) << in;
    else if constexpr(std::is_same_v<T, double>) ss << std::fixed << std::setprecision(14) << in;
    else                                         ss << in;
    return ss.str();
}

// boolean type to string
template<> string serialize<bool>(const bool& in) {
    return in ? "true" : "false";
}

// string to string
template<> string serialize<string>(const string& in) {
    string out;
    out.append("\"");
    size_t loop = in.size();
    for(size_t i = 0; i < loop; ++i) {
        switch(in[i]) {
            case '\\': out.append("\\\\");   break;
            case '\"': out.append("\\\"");   break;
            case '\n': out.append("\\n");    break;
            case '\t': out.append("\\t");    break;
            case '[': out.append("\\[");    break;
            case ']': out.append("\\]");    break;
            case '{': out.append("\\{");    break;
            case '}': out.append("\\}");    break;
            default:   out.append(1, in[i]); break;
          }
    }
    out.append("\"");
    return out;
}

// container to string
template<> string serialize<Container>(const Container& in) {
    return in.serialize();
}

// string to primitive type
template<typename T> T deserialize(const string& in) {
    // TODO: exeption
    // 1. out of range
    // 2. format mismatch
    // 3. type mismatch ?

    size_t pos = 0;
    while (true) {
        if (in[pos] == '\0' || in[pos] == ',') {
            break;
        }
        ++pos;
    }
    std::stringstream ss{ in.substr(0, pos) };
    T result;
    ss >> result;
    return result;
}

// string to boolean type
template<> bool deserialize<bool>(const string& in) {
    // TODO: exeption
    // not "true" or "false"

    if(in[0] == 't')  {
        return true;
    }
    return false;
}

// string to string
template<> string deserialize<string>(const string& in) {
    string result;
    if(in[0] != '\"') {
        assert(false);
    }
    size_t pos = in.rfind('\"');
    for(size_t i =  1; i < pos; ++i) {
        if(in[i] == '\\') {
            switch (in[i + 1]) {
                case '\\': result.append("\\"); break;
                case '\"': result.append("\""); break;
                case '[':  result.append("["); break;
                case ']':  result.append("]"); break;
                case '{':  result.append("{"); break;
                case '}':  result.append("}"); break;

                // \n, \t, etc...
                default:
                    result.append(1, '\\');
                    result.append(1, in[i + 1]);
                    break;
            }
            ++i;
        }
        else result.append(1, in[i]);
    }
    return result;
}

// string to container
void deserialize(Container* out, const string& in) {
    out->deserialize(in);
}

// runtime stringify
void serialize(std::string* out, const void* in, const Keyword& type) {
    switch (type) {
    // skip
    case Keyword::UNREGISTERED:
    case Keyword::VOID: 
        //TODO:
        break;

    case Keyword::CLASS:  
        out->append(reinterpret_cast<const Object*>(in)->stringify());
        break;

    case Keyword::POINTER: {
        //TODO:
        }
        break;

    case Keyword::REFERENCE: {
        //TODO:
        }
        break;

        out->append(serialize(*static_cast<const bool*>(in)));
        break;

    // int
    case Keyword::SIGNED_INT:
    case Keyword::UNSIGNED_INT:
        out->append(serialize(*static_cast<const int*>(in)));
        break;

    case Keyword::BOOL:
        out->append(serialize(*static_cast<const int*>(in)));
        break;

    // char
    case Keyword::CHAR:
    case Keyword::SIGNED_CHAR:
    case Keyword::UNSIGNED_CHAR:
        out->append(serialize(*static_cast<const char*>(in)));
        break;

    // wchar
    case Keyword::WCHAR_T:
        //TODO: fix
        // stringify<wchar_t>(out, in);
        break;

    // short
    case Keyword::SIGNED_SHORT:
    case Keyword::UNSIGNED_SHORT: 
        out->append(serialize(*static_cast<const short*>(in)));
        break;

    // long
    case Keyword::SIGNED_LONG:
    case Keyword::UNSIGNED_LONG: 
        out->append(serialize(*static_cast<const long*>(in)));
        break;

    // long long
    case Keyword::SIGNED_LONG_LONG:
    case Keyword::UNSIGNED_LONG_LONG:
        out->append(serialize(*static_cast<const long long*>(in)));
        break;
    
    // float
    case Keyword::FLOAT:
        out->append(serialize<float>(*static_cast<const float*>(in))); 
        break;

    // double
    case Keyword::DOUBLE: 
        out->append(serialize<double>(*static_cast<const double*>(in)));
        break;

    // long double
    case Keyword::LONG_DOUBLE: 
        out->append(serialize<long double>(*static_cast<const long double*>(in)));
        break;
    
    case Keyword::ENUM:
        // out->append(static_cast<const EInterface*>(in)->stringify());
        break;

    // function
    case Keyword::FUNCTION: 
        //TODO:
        break;

    case Keyword::STD_STRING: 
        out->append(serialize(*static_cast<const string*>(in)));
        break;

    case Keyword::STL_DEQUE:
        out->append(serialize(*static_cast<const Container*>(in)));
        break;
    }
}

// runtime parse
void deserialize(void* out, const std::string& in, const Keyword& type) {
    switch (type) {
    case Keyword::UNREGISTERED:
    case Keyword::VOID:
        break;

    case Keyword::CLASS: 
        Object::parse(static_cast<Object*>(out), in);
        break;

    case Keyword::POINTER: {
        //TODO:
        }
        break;

    case Keyword::REFERENCE: {
        //TODO:
        }
        break;

    case Keyword::UNION: {
        //TODO:
        }
        break;

    // int
    case Keyword::SIGNED_INT:
    case Keyword::UNSIGNED_INT: 
        *static_cast<int*>(out) = deserialize<int>(in);
        break;

    // bool
    case Keyword::BOOL:
        *static_cast<bool*>(out) = deserialize<bool>(in);
    break;

    // char
    case Keyword::CHAR:
    case Keyword::SIGNED_CHAR:
    case Keyword::UNSIGNED_CHAR:
        *static_cast<char*>(out) = deserialize<char>(in);
        break;

    // wchar
    case Keyword::WCHAR_T:
    // TODO: fix
        // parse<wchar_t>(static_cast<wchar_t*>(out), in, inout);
        break;

    // short
    case Keyword::SIGNED_SHORT:
    case Keyword::UNSIGNED_SHORT: 
        *static_cast<short*>(out) = deserialize<short>(in);
        break;

    // long
    case Keyword::SIGNED_LONG:
    case Keyword::UNSIGNED_LONG: 
        *static_cast<long*>(out) = deserialize<long>(in);
        break;

    // long long
    case Keyword::SIGNED_LONG_LONG:
    case Keyword::UNSIGNED_LONG_LONG:
        *static_cast<long long*>(out) = deserialize<long long>(in);
        break;

    // float
    case Keyword::FLOAT:
        *static_cast<float*>(out) = deserialize<float>(in);
        break;

    // double
    case Keyword::DOUBLE:
        *static_cast<double*>(out) = deserialize<double>(in);
        break;

    // long double    
    case Keyword::LONG_DOUBLE:
        *static_cast<long double*>(out) = deserialize<long double>(in);
        break;

    case Keyword::ENUM:
        // static_cast<EInterface*>(out)->parse(in);
        break;

    // function
    case Keyword::FUNCTION:
        //TODO:
        break;

    // std::string
    case Keyword::STD_STRING: 
        *static_cast<string*>(out) = deserialize<string>(in);
        break;

    case Keyword::STL_DEQUE:
        static_cast<Container*>(out)->deserialize(in);
        break;
    }
}

/*
 * STL
 */

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