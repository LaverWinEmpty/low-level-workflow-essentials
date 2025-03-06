#ifndef LWE_SERIALIZE_HEADER
#define LWE_SERIALIZE_HEADER

#include <sstream>
#include <iomanip>
#include "meta.hpp"
#include "container.hpp"

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
            default:   out.append(1, in[i]); break;
          }
    }
    out.append("\"");
    return out;
}

// container serialize
template<> string serialize<LWE::stl::Container>(const LWE::stl::Container& in) {
    return in.tostring();
}

// primitive type from string
template<typename T> T deserialize(const string& in, size_t* out = nullptr) {
    // TODO: exeption
    // 1. out of range
    // 2. format mismatch
    // 3. type mismatch ?

    /*size_t pos = in.find_first_of(",\0");
    if(out) {
        *out = pos;
    }*/

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


// boolean type from string
template<> bool deserialize<bool>(const string& in, size_t* out) {
    // TODO: exeption
    // not "true" or "false"

    if(in[0] == 't')  {
        if(out) {
            *out += 4;
        }
        return true;
    }
    if(out) {
        *out += 5;
    }
    return false;
}

// string from string
template<> string deserialize<string>(const string& in, size_t* out) {
    string result;
    if(in[0] != '\"') {
        assert(false);
    }
    size_t pos = in.find('\"', 1);
    for(size_t i =  1; i < pos; ++i) {
        if(in[i] == '\\') {
            switch (in[i + 1]) {
                case '\\': result.append("\\"); break;
                case '\"': result.append("\""); break;

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

// runtime serialize
void serialize(std::string* out, const void* in, const MetaType& type) {
    switch (type) {
    // skip
    case MetaType::UNREGISTERED:
    case MetaType::VOID: 
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
        out->append(serialize(*static_cast<const int*>(in)));
        break;

    // bool
    case MetaType::BOOL:
        out->append(serialize(*static_cast<const bool*>(in)));
        break;

    // char
    case MetaType::CHAR:
    case MetaType::SIGNED_CHAR:
    case MetaType::UNSIGNED_CHAR:
        out->append(serialize(*static_cast<const char*>(in)));
        break;

    // wchar
    case MetaType::WCHAR:
        //TODO: fix
        // serialize<wchar_t>(out, in);
        break;

    // short
    case MetaType::SIGNED_SHORT:
    case MetaType::UNSIGNED_SHORT: 
        out->append(serialize(*static_cast<const short*>(in)));
        break;

    // long
    case MetaType::SIGNED_LONG:
    case MetaType::UNSIGNED_LONG: 
        out->append(serialize(*static_cast<const long*>(in)));
        break;

    // long long
    case MetaType::SIGNED_LONG_LONG:
    case MetaType::UNSIGNED_LONG_LONG:
        out->append(serialize(*static_cast<const long long*>(in)));
        break;
    
    // float
    case MetaType::FLOAT:
        out->append(serialize<float>(*static_cast<const float*>(in))); 
        break;

    // double
    case MetaType::DOUBLE: 
        out->append(serialize<double>(*static_cast<const double*>(in)));
        break;

    // long double
    case MetaType::LONG_DOUBLE: 
        out->append(serialize<long double>(*static_cast<const long double*>(in)));
        break;
    
    // function
    case MetaType::FUNCTION: 
        //TODO:
        break;

    case MetaType::STD_STRING: 
        out->append(serialize(*static_cast<const string*>(in)));
        break;

    case MetaType::STL_DEQUE:
        out->append(serialize(*static_cast<const LWE::stl::Container*>(in)));
        break;
    }
}

// runtime deserialize
void deserialize(void* out, const std::string& in, const MetaType& type, size_t& inout) {
    size_t next = 0;

    switch (type) {
    case MetaType::UNREGISTERED:
    case MetaType::VOID:
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
        *static_cast<int*>(out) = deserialize<int>(in.substr(inout), &next);
        break;

    // bool
    case MetaType::BOOL:
        *static_cast<bool*>(out) = deserialize<bool>(in.substr(inout), &next);
    break;

    // char
    case MetaType::CHAR:
    case MetaType::SIGNED_CHAR:
    case MetaType::UNSIGNED_CHAR:
        *static_cast<char*>(out) = deserialize<char>(in.substr(inout), &next);
        break;

    // wchar
    case MetaType::WCHAR:
    // TODO: fix
        // deserialize<wchar_t>(static_cast<wchar_t*>(out), in, inout);
        break;

    // short
    case MetaType::SIGNED_SHORT:
    case MetaType::UNSIGNED_SHORT: 
        *static_cast<short*>(out) = deserialize<short>(in.substr(inout), &next);
        break;

    // long
    case MetaType::SIGNED_LONG:
    case MetaType::UNSIGNED_LONG: 
        *static_cast<long*>(out) = deserialize<long>(in.substr(inout), &next);
        break;

    // long long
    case MetaType::SIGNED_LONG_LONG:
    case MetaType::UNSIGNED_LONG_LONG:
        *static_cast<long long*>(out) = deserialize<long long>(in.substr(inout), &next);
        break;

    // float
    case MetaType::FLOAT:
        *static_cast<float*>(out) = deserialize<float>(in.substr(inout), &next);
        break;

    // double
    case MetaType::DOUBLE:
        *static_cast<double*>(out) = deserialize<double>(in.substr(inout), &next);
        break;

    // long double    
    case MetaType::LONG_DOUBLE:
        *static_cast<long double*>(out) = deserialize<long double>(in.substr(inout), &next);
        break;

    case MetaType::STD_STRING: 
        *static_cast<string*>(out) = deserialize<string>(in.substr(inout), &next);
        break;
    }
    
    inout += next;
}

// clang-format on

// template<typename T> std::string serialize(const T& in) {
//     std::string string;
//     serialize(&string, reinterpret_cast<const void*>(&in), typecode<T>());
//     return string;
// }

template<typename T> void deserialize(T* out, const std::string& in) {
    size_t idx = 0;
    deserialize(out, in, typecode<T>(), idx);
}

#endif