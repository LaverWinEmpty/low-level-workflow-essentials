#ifdef LWE_META_SERIALIZE
LWE_BEGIN
namespace meta {

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

// container to string
template<typename Derived> string serialize(const Container& in) {
    std::string out;

    // CRTP begin / end
    typename Derived::Iterator curr = in.begin();
    typename Derived::Iterator last = in.end();

    // has data
    if(curr != last) {
        out.append("[");
        // for each
        while(true) {
            serialize(&out, &*curr, typecode<typename Derived::value_type>());
            ++curr;
            if(curr != last) {
                out.append(", ");
            } else break;
        }
        out.append("]");
    } else return "[]";
    return out;
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
void deserialize() {
    out->deserialize(in);
}

// string to container
template<typename Dervied>void deserialize(Container* out, const string& in) {
    using Element = typename Derived::value_type;
    if(in == "[]") {
        return Derived{}; // empty
    }
    Derived out; // else

    size_t begin = 1;             // "[", ignore 1
    size_t end   = in.size() - 1; // "]", ignore 1
    size_t len   = 0;

    // parsing
    size_t i = begin;
    for(; i < end; ++i, ++len) {
        if constexpr(std::is_same_v<Element, string>) {
            // find <",> but ignore \"
            if(in[i] == '\"' && in[i + 1] == ',', in[i - 1] != '\\') {
                Element data;
                // len + 1: with '\"'
                meta::deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len + 1), typecode<Element>());
                i     += 3; // pass <", >
                begin  = i; // next position
                len    = 0; // next length
                out.push(std::move(data));
            }
        }

        else if constexpr(isSTL<Element>()) {
            // find <],> but ignore \]
            if(in[i] == ']' && in[i + 1] == ',' && in[i - 1] != '\\') {
                Element data;
                // len + 1: with ']'
                meta::deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len + 1), typecode<Element>());
                i     += 3; // pass <], >
                begin  = i; // next position
                len    = 0; // next length
                out.push(std::move(data));
            }
        }

        else if constexpr(std::is_same_v<Element, Object> || std::is_base_of_v<Object, Element>) {
            if(in[i] == '}' && in[i + 1] == ',' && in[i - 1] != '\\') {
                Element data;
                meta::deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
                i     += 2; // pass <, >
                begin  = i; // next position
                len    = 0; // next length
                out.push(std::move(data));
            }
        }

        else if(in[i] == ',') {
            Element data;
            meta::deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
            i     += 2; // pass <, >
            begin  = i; // next position
            len    = 0; // next length
            out.push(std::move(data));
        }
    }

    // insert last data
    Element data;
    meta::deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
    out.push(std::move(data));
    return std::move(out);
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

}
LWE_END
#endif