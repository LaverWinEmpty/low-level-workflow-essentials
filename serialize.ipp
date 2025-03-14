#ifndef LWE_SERIALIZE_HEADER
#    include "serialize.hpp"
#endif

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
template<> string serialize<LWE::stl::Container>(const LWE::stl::Container& in) {
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
void deserialize(LWE::stl::Container* out, const string& in) {
    out->deserialize(in);
}

// runtime serialize
void serialize(std::string* out, const void* in, const EType& type) {
    switch (type) {
    // skip
    case EType::UNREGISTERED:
    case EType::VOID: 
        //TODO:
        break;

    case EType::CLASS: {
        //TODO:
        }
        break;

    case EType::POINTER: {
        //TODO:
        }
        break;

    case EType::REFERENCE: {
        //TODO:
        }
        break;

    case EType::UNION: {
        //TODO:
        }
        break;

    // int
    case EType::SIGNED_INT:
    case EType::UNSIGNED_INT:
        out->append(serialize(*static_cast<const int*>(in)));
        break;

    // bool
    case EType::BOOL:
        out->append(serialize(*static_cast<const bool*>(in)));
        break;

    // char
    case EType::CHAR:
    case EType::SIGNED_CHAR:
    case EType::UNSIGNED_CHAR:
        out->append(serialize(*static_cast<const char*>(in)));
        break;

    // wchar
    case EType::WCHAR_T:
        //TODO: fix
        // serialize<wchar_t>(out, in);
        break;

    // short
    case EType::SIGNED_SHORT:
    case EType::UNSIGNED_SHORT: 
        out->append(serialize(*static_cast<const short*>(in)));
        break;

    // long
    case EType::SIGNED_LONG:
    case EType::UNSIGNED_LONG: 
        out->append(serialize(*static_cast<const long*>(in)));
        break;

    // long long
    case EType::SIGNED_LONG_LONG:
    case EType::UNSIGNED_LONG_LONG:
        out->append(serialize(*static_cast<const long long*>(in)));
        break;
    
    // float
    case EType::FLOAT:
        out->append(serialize<float>(*static_cast<const float*>(in))); 
        break;

    // double
    case EType::DOUBLE: 
        out->append(serialize<double>(*static_cast<const double*>(in)));
        break;

    // long double
    case EType::LONG_DOUBLE: 
        out->append(serialize<long double>(*static_cast<const long double*>(in)));
        break;
    
    // function
    case EType::FUNCTION: 
        //TODO:
        break;

    case EType::STD_STRING: 
        out->append(serialize(*static_cast<const string*>(in)));
        break;

    case EType::STL_DEQUE:
        out->append(serialize(*static_cast<const LWE::stl::Container*>(in)));
        break;
    }
}

// runtime deserialize
void deserialize(void* out, const std::string& in, const EType& type) {
    switch (type) {
    case EType::UNREGISTERED:
    case EType::VOID:
        break;

    case EType::CLASS: {
        //TODO:
        }
        break;

    case EType::POINTER: {
        //TODO:
       }
        break;

    case EType::REFERENCE: {
        //TODO:
        }
        break;

    case EType::UNION: {
        //TODO:
        }
        break;

    // int
    case EType::SIGNED_INT:
    case EType::UNSIGNED_INT: 
        *static_cast<int*>(out) = deserialize<int>(in);
        break;

    // bool
    case EType::BOOL:
        *static_cast<bool*>(out) = deserialize<bool>(in);
    break;

    // char
    case EType::CHAR:
    case EType::SIGNED_CHAR:
    case EType::UNSIGNED_CHAR:
        *static_cast<char*>(out) = deserialize<char>(in);
        break;

    // wchar
    case EType::WCHAR_T:
    // TODO: fix
        // deserialize<wchar_t>(static_cast<wchar_t*>(out), in, inout);
        break;

    // short
    case EType::SIGNED_SHORT:
    case EType::UNSIGNED_SHORT: 
        *static_cast<short*>(out) = deserialize<short>(in);
        break;

    // long
    case EType::SIGNED_LONG:
    case EType::UNSIGNED_LONG: 
        *static_cast<long*>(out) = deserialize<long>(in);
        break;

    // long long
    case EType::SIGNED_LONG_LONG:
    case EType::UNSIGNED_LONG_LONG:
        *static_cast<long long*>(out) = deserialize<long long>(in);
        break;

    // float
    case EType::FLOAT:
        *static_cast<float*>(out) = deserialize<float>(in);
        break;

    // double
    case EType::DOUBLE:
        *static_cast<double*>(out) = deserialize<double>(in);
        break;

    // long double    
    case EType::LONG_DOUBLE:
        *static_cast<long double*>(out) = deserialize<long double>(in);
        break;

    // std::string
    case EType::STD_STRING: 
        *static_cast<string*>(out) = deserialize<string>(in);
        break;

    case EType::STL_DEQUE:
        static_cast<LWE::stl::Container*>(out)->deserialize(in);
        break;
    }
}