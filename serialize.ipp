#ifndef LWE_SERIALIZE_HEADER
#    include "serialize.hpp"
#endif

// clang-format off

// primitive type to string
template<typename T> string tostr(const T& in) {
    std::stringstream ss;
    if constexpr(std::is_same_v<T, float>)       ss << std::fixed << std::setprecision(6) << in;
    else if constexpr(std::is_same_v<T, double>) ss << std::fixed << std::setprecision(14) << in;
    else                                         ss << in;
    return ss.str();
}

// boolean type to string
template<> string tostr<bool>(const bool& in) {
    return in ? "true" : "false";
}

// string to string
template<> string tostr<string>(const string& in) {
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

// container to string
template<> string tostr<LWE::stl::Container>(const LWE::stl::Container& in) {
    return in.serialize();
}

// string to primitive type
template<typename T> T fromstr(const string& in) {
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

// string to boolean type
template<> bool fromstr<bool>(const string& in) {
    // TODO: exeption
    // not "true" or "false"

    if(in[0] == 't')  {
        return true;
    }
    return false;
}

// string to string
template<> string fromstr<string>(const string& in) {
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
void fromstr(LWE::stl::Container* out, const string& in) {
    out->deserialize(in);
}

// runtime serialize
void tostr(std::string* out, const void* in, const MetaType& type) {
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
        out->append(tostr(*static_cast<const int*>(in)));
        break;

    // bool
    case MetaType::BOOL:
        out->append(tostr(*static_cast<const bool*>(in)));
        break;

    // char
    case MetaType::CHAR:
    case MetaType::SIGNED_CHAR:
    case MetaType::UNSIGNED_CHAR:
        out->append(tostr(*static_cast<const char*>(in)));
        break;

    // wchar
    case MetaType::WCHAR_T:
        //TODO: fix
        // serialize<wchar_t>(out, in);
        break;

    // short
    case MetaType::SIGNED_SHORT:
    case MetaType::UNSIGNED_SHORT: 
        out->append(tostr(*static_cast<const short*>(in)));
        break;

    // long
    case MetaType::SIGNED_LONG:
    case MetaType::UNSIGNED_LONG: 
        out->append(tostr(*static_cast<const long*>(in)));
        break;

    // long long
    case MetaType::SIGNED_LONG_LONG:
    case MetaType::UNSIGNED_LONG_LONG:
        out->append(tostr(*static_cast<const long long*>(in)));
        break;
    
    // float
    case MetaType::FLOAT:
        out->append(tostr<float>(*static_cast<const float*>(in))); 
        break;

    // double
    case MetaType::DOUBLE: 
        out->append(tostr<double>(*static_cast<const double*>(in)));
        break;

    // long double
    case MetaType::LONG_DOUBLE: 
        out->append(tostr<long double>(*static_cast<const long double*>(in)));
        break;
    
    // function
    case MetaType::FUNCTION: 
        //TODO:
        break;

    case MetaType::STD_STRING: 
        out->append(tostr(*static_cast<const string*>(in)));
        break;

    case MetaType::STL_DEQUE:
        out->append(tostr(*static_cast<const LWE::stl::Container*>(in)));
        break;
    }
}

// runtime deserialize
void fromstr(void* out, const std::string& in, const MetaType& type) {
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
        *static_cast<int*>(out) = fromstr<int>(in);
        break;

    // bool
    case MetaType::BOOL:
        *static_cast<bool*>(out) = fromstr<bool>(in);
    break;

    // char
    case MetaType::CHAR:
    case MetaType::SIGNED_CHAR:
    case MetaType::UNSIGNED_CHAR:
        *static_cast<char*>(out) = fromstr<char>(in);
        break;

    // wchar
    case MetaType::WCHAR_T:
    // TODO: fix
        // deserialize<wchar_t>(static_cast<wchar_t*>(out), in, inout);
        break;

    // short
    case MetaType::SIGNED_SHORT:
    case MetaType::UNSIGNED_SHORT: 
        *static_cast<short*>(out) = fromstr<short>(in);
        break;

    // long
    case MetaType::SIGNED_LONG:
    case MetaType::UNSIGNED_LONG: 
        *static_cast<long*>(out) = fromstr<long>(in);
        break;

    // long long
    case MetaType::SIGNED_LONG_LONG:
    case MetaType::UNSIGNED_LONG_LONG:
        *static_cast<long long*>(out) = fromstr<long long>(in);
        break;

    // float
    case MetaType::FLOAT:
        *static_cast<float*>(out) = fromstr<float>(in);
        break;

    // double
    case MetaType::DOUBLE:
        *static_cast<double*>(out) = fromstr<double>(in);
        break;

    // long double    
    case MetaType::LONG_DOUBLE:
        *static_cast<long double*>(out) = fromstr<long double>(in);
        break;

    // std::string
    case MetaType::STD_STRING: 
        *static_cast<string*>(out) = fromstr<string>(in);
        break;

    case MetaType::STL_DEQUE:
        static_cast<LWE::stl::Container*>(out)->deserialize(in);
        break;
    }
}