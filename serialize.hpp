#ifndef LWE_SERIALIZE_HEADER
#define LWE_SERIALIZE_HEADER

#include "meta.hpp"
#include "container.hpp"

// primitive type to string
template<typename T> void serialize(std::string* out, void* in) {
    string to = std::to_string(*reinterpret_cast<T*>(in));
    out->append(to);
}

// clang-format off
void serialize(std::string* out, void* in, const MetaType& type) {
    switch (type) {
    // skip
    case MetaType::VOID: 
    case MetaType::UNREGISTERED:
        //TODO:
    break;

    case MetaType::CLASS: {
        //TODO:
    }
    break;

    case MetaType::POINTER: {
        //TODO:
    }
    break;

    case MetaType::REFERENCE: {
        //TODO:
    }
    break;

    case MetaType::UNION: {
        //TODO:
    }
    break;

    // int
    case MetaType::SIGNED_INT:
    case MetaType::UNSIGNED_INT:
        serialize<int>(out, in);
        break;

    // bool
    case MetaType::BOOL:
        serialize<bool>(out, in);
    break;

    // char
    case MetaType::CHAR:
    case MetaType::SIGNED_CHAR:
    case MetaType::UNSIGNED_CHAR:
        serialize<char>(out, in);
    break;

    // wchar
    case MetaType::WCHAR:
        serialize<wchar_t>(out, in);
    break;

    // short
    case MetaType::SIGNED_SHORT:
    case MetaType::UNSIGNED_SHORT: 
        serialize<short>(out, in);
    break;

    // long
    case MetaType::SIGNED_LONG:
    case MetaType::UNSIGNED_LONG: 
        serialize<long>(out, in);
    break;

    // long long
    case MetaType::SIGNED_LONG_LONG:
    case MetaType::UNSIGNED_LONG_LONG:
        serialize<long long>(out, in);
    break;

    // float
    case MetaType::FLOAT: 
        serialize<float>(out, in);
    break;

    // double
    case MetaType::DOUBLE: 
        serialize<double>(out, in);
    break;

    // long double
    case MetaType::LONG_DOUBLE: 
        serialize<long double>(out, in);
    break;
    
    // function
    case MetaType::FUNCTION: {
        //TODO:
    }
    break;

    case MetaType::STD_STRING: {
            std::string* ptr = reinterpret_cast<std::string*>(in);
            out->append("\"");
            size_t loop = ptr->size();
            for(size_t i = 0; i < loop; ++i) {
                switch((*ptr)[i]) {
                    case '\\': out->append("\\\\");       break;
                    case '\"': out->append("\\\"");       break;
                    case '\n': out->append("\\n");        break;
                    case '\t': out->append("\\t");        break;
                    default:   out->append(1, (*ptr)[i]); break;
                }
            }
            out->append("\"");
        }
        break;

    case MetaType::STL_DEQUE: {
            lwe::stl::Container* ptr = reinterpret_cast<LWE::stl::Container*>(in);
            out->append(ptr->tostring());
        }
        break;
    }

    // primitive
}
// clang-format on

template<typename T> std::string serialize(const T& in, const MetaType& type) {
    std::string string;
    serializeREC(string, in, type);
    return string;
}

#endif