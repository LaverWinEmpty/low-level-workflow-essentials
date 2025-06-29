#ifndef LWE_META_CODEC
#define LWE_META_CODEC

#include "internal/feature.hpp"
#include "object.hpp"
#include "../base/base.h"
#include "../mem/ptr.hpp"

LWE_BEGIN
namespace meta {

class Object;

class Codec: Static {
public:
    template<typename T> static string from(const T&);    //!< (to string) from T
    template<typename T> static T      to(const string&); //!< to T (from string)

public:
    template<typename T> static string     encode(const Container&);          //!< container encoder
    template<typename Derived> static void decode(Container*, const string&); //!< container decoder

public:
    static string encode(const Object&);          //!< Object encoder
    static void   decode(Object*, const string&); //!< Object decoder

private:
    static void encode(string*, const void*, const Keyword&);      //!< run-time serialize
    static void decode(void*, const std::string&, const Keyword&); //!< run-time deserialize

private:
    //! @brief check parse end
    template<typename T> static bool parsed(const string&, size_t);
};

/**************************************************************************************************
 * default
 **************************************************************************************************/
// serialize
template<typename T> string Codec::from(const T& in) {
    // objcet or container
    if constexpr(isSTL<T>() || isOBJ<T>()) {
        return in.serialize();
    }
    else {
        std::stringstream ss;
        if constexpr(std::is_same_v<T, float>) ss << std::fixed << std::setprecision(6) << in;
        else if constexpr(std::is_same_v<T, double>) ss << std::fixed << std::setprecision(14) << in;
        else if constexpr(std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) ss << int(in);
        else ss << in;
        return ss.str();
    }
}

// deserialize
template<typename T> T Codec::to(const string& in) {
    // object or container
    if constexpr(isSTL<T>() || isOBJ<T>()) {
        T data;
        data.deserialize(in);
        return std::move(data);
    }

    // TODO: exeption
    // 1. out of range
    // 2. format mismatch
    // 3. type mismatch ?

    // string to primitive type
    else {
        size_t pos = 0;
        while(true) {
            if(in[pos] == '\0' || (in[pos] == ',' && parsed<T>(in, pos))) {
                break;
            }
            ++pos;
        }
        std::stringstream ss{ in.substr(0, pos) };

        T result;
        if constexpr (std::is_same_v<int8_t, T> || std::is_same_v<uint8_t, T>) {
            int temp;
            ss >> temp;
            result = T(temp);
        }
        else ss >> result;
        return result;
    }
}

/**************************************************************************************************
 * character specialization
 **************************************************************************************************/
// serialize
template<> string Codec::from<char>(const char& in) {
    switch(in) {
        case '\\': return ("\\\\");
        case '\"': return ("\\\"");
        case '\n': return ("\\n");
        case '\t': return ("\\t");
        case '[':  return ("\\[");
        case ']':  return ("\\]");
        case '{':  return ("\\{");
        case '}':  return ("\\}");
        case ',':  return ("\\,");
        default:   return string(1, in);
    }
}

// deserialize
template<> char Codec::to<char>(const string& in) {
    if(in[0] == '\\') {
        switch(in[1]) {
            case '\\': return '\\';
            case '\"': return '\"';
            case '[':  return '[';
            case ']':  return ']';
            case '{':  return '{';
            case '}':  return '}';
            case ',':  return ',';
            case 'n':  return '\n';
            case 't':  return '\t';
        }
    }
    return in[0];
}

/**************************************************************************************************
 * bool specialization
 **************************************************************************************************/
// serialize
template<> string Codec::from<bool>(const bool& in) {
    return in ? "true" : "false";
}

// deserialize
template<> bool Codec::to<bool>(const string& in) {
    // TODO: exeption not "true" or "false"
    if(in[0] == 't') {
        return true;
    }
    return false;
}

/**************************************************************************************************
 * string specialization
 **************************************************************************************************/
// serialize
template<> string Codec::from<string>(const string& in) {
    string out;
    out.append("\"");
    size_t loop = in.size();
    for(size_t i = 0; i < loop; ++i) {
        out.append(from<char>(in[i]));
    }
    out.append("\"");
    return out;
}

// deserialize
template<> string Codec::to<string>(const string& in) {
    string result;
    if(in[0] != '\"') {
        throw diag::error(diag::INVALID_DATA);
    }
    size_t pos = in.rfind('\"');
    for(size_t i = 1; i < pos; ++i) {
        if(in[i] == '\\') {
            result.append(1, to<char>(in.substr(i, 2)));
            ++i;
        }
        else result.append(1, to<char>(in.substr(i, 1)));
    }
    return result;
}

/**************************************************************************************************
 * container detail
 **************************************************************************************************/
// serialize
template<typename T> string Codec::encode(const Container& in) {
    std::string out;

    // CRTP begin / end
    typename T::Iterator curr = reinterpret_cast<const T&>(in).begin();
    typename T::Iterator last = reinterpret_cast<const T&>(in).end();

    // has data
    if(curr != last) {
        out.append("[ ");
        // for each
        while(true) {
            encode(&out, &*curr, typecode<typename T::value_type>());
            ++curr;
            if(curr != last) {
                out.append(", ");
            }
            else break;
        }
        out.append(" ]");
    }
    else return "[]";
    return out;
}

// deserialize
template<typename Derived> void Codec::decode(Container* ptr, const string& in) {
    using Element = typename Derived::value_type;
    if(in == "[]") {
        return; // empty
    }
    Derived& out = *reinterpret_cast<Derived*>(ptr); // else

    size_t begin = 2;             // "[ ", ignore 2
    size_t end   = in.size() - 2; // " ]", ignore 2
    size_t len   = 0;

    // parsing
    size_t i = begin;
    for(; i < end; ++i, ++len) {
        // string / container / object
        if constexpr(std::is_same_v<Element, string> || isSTL<Element>() || isOBJ<Element>()) {
            // find `,`
            if(parsed<Element>(in, i)) {
                Element data;
                // len + 1: with word to pack (\" or \} or \])
                decode(reinterpret_cast<void*>(&data), in.substr(begin, len + 1), typecode<Element>());
                i     += 3; // pass `", `
                begin  = i; // next position
                len    = 0; // next length
                out.push(std::move(data));
            }
        }

        // character
        else if constexpr(std::is_same_v<Element, char>) {
            if(parsed<Element>(in, i)) {
                Element data;
                decode(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
                i     += 2; // pass `, `
                begin  = i; // next position
                len    = 0; // next length
                out.push(std::move(data));
            }
        }

        // primitive
        else if(parsed<Element>(in, i)) {
            Element data;
            decode(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
            i     += 2; // pass <, >
            begin  = i; // next position
            len    = 0; // next length
            out.push(std::move(data));
        }
    }

    // insert last data
    Element data;
    decode(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
    out.push(std::move(data));
}

/**************************************************************************************************
 * object deail
 **************************************************************************************************/
string Codec::encode(const Object& in) {
    const Structure& prop = in.meta()->fields();
    if(prop.size() == 0) {
        return "{}";
    }

    string buffer;
    buffer.reserve(4'096);

    char*  ptr  = const_cast<char*>(reinterpret_cast<const char*>(&in));
    size_t loop = prop.size() - 1;
    buffer.append("{ ");
    for(size_t i = 0; i < loop; ++i) {
        encode(&buffer, ptr + prop[i].offset, prop[i].type.code());
        buffer.append(", ");
    }
    encode(&buffer, ptr + prop[loop].offset, prop[loop].type.code());
    buffer.append(" }");
    return buffer;
}

// deserialize
void Codec::decode(Object* out, const string& in) {
    char* ptr = const_cast<char*>(reinterpret_cast<const char*>(out));

    // empty
    if(in == "{}") {
        return;
    }

    const Structure& prop = out->meta()->fields();
    if(prop.size() == 0) {
        assert(false);
    }

    size_t begin = 2; // "{ ", ignore 2
    size_t len   = 0;

    size_t loop = prop.size();
    for(size_t i = 0; i < loop; ++i) {
        if(isSTL(prop[i].type.code())) {
            len = 1; // pass '['
            while(true) {
                if(in[begin + len] == ']' && in[begin + len - 1] != '\\') {
                    break;
                }
                ++len;
            }
            // len + 1: with ']'
            decode(ptr + prop[i].offset, in.substr(begin, len + 1), prop[i].type.code());
            begin += (len + 3); // pass `], `
            len    = 0;
        }

        else if(prop[i].type == Keyword::STD_STRING) {
            len = 1; // pass '\"'
            while(true) {
                if(in[begin + len] == '\"' && in[begin + len - 1] != '\\') {
                    break;
                }
                ++len;
            }
            // len + 1: with '\"'
            decode(ptr + prop[i].offset, in.substr(begin, len + 1), prop[i].type.code());
            begin += (len + 3); // pass `", `
            len    = 0;
        }

        else if(prop[i].type == Keyword::CLASS) {
            len = 1; // pass "{ "

            while(true) {
                if(in[begin + len] == '}' && in[begin + len - 1] != '\\') {
                    break;
                }
                ++len;
            }
            // len + 1: with '}'
            decode(ptr + prop[i].offset, in.substr(begin, len + 1), prop[i].type.code());
            begin += (len + 3); // pass `}, `
            len    = 0;
        }

        // primitive: integer or floating
        else {
            // find <, > or < }>
            while(true) {
                if((in[begin + len] == ',' && in[begin + len + 1] == ' ') ||
                   (in[begin + len] == ' ' && in[begin + len + 1] == '}')) {
                    break;
                }
                ++len;
            }
            decode(ptr + prop[i].offset, in.substr(begin, len), prop[i].type.code()); // ignore ',' or ' '
            begin += (len + 2);                                                       // pass <, > or < ]>
            len    = 0;
        }
    }
}

// definition
string Object::serialize() const {
    return Codec::encode(*this);
}
void Object::deserialize(const std::string& in) {
    Codec::decode(this, in);
}
void Object::deserialize(Object* out, const std::string& in) {
    out->deserialize(in);
}

/**************************************************************************************************
 * run-time serialization
 **************************************************************************************************/
// serialize
void Codec::encode(std::string* out, const void* in, const Keyword& type) {
    switch(type) {
        // skip
        case Keyword::UNREGISTERED:
        case Keyword::VOID:
            // TODO:
            break;

        case Keyword::POINTER: {
            // TODO:
        } break;

        case Keyword::REFERENCE: {
            // TODO:
        } break;

        // integer
        case Keyword::BOOL:               out->append(from(*static_cast<const bool*>(in))); break;
        case Keyword::CHAR:               out->append(from(*static_cast<const char*>(in))); break;
        case Keyword::SIGNED_INT:         out->append(from(*static_cast<const signed int*>(in))); break;
        case Keyword::SIGNED_SHORT:       out->append(from(*static_cast<const signed short*>(in))); break;
        case Keyword::SIGNED_LONG:        out->append(from(*static_cast<const signed long*>(in))); break;
        case Keyword::SIGNED_CHAR:        out->append(from(*static_cast<const signed char*>(in))); break;
        case Keyword::SIGNED_LONG_LONG:   out->append(from(*static_cast<const signed long long*>(in))); break;
        case Keyword::UNSIGNED_INT:       out->append(from(*static_cast<const unsigned int*>(in))); break;
        case Keyword::UNSIGNED_CHAR:      out->append(from(*static_cast<const unsigned char*>(in))); break;
        case Keyword::UNSIGNED_SHORT:     out->append(from(*static_cast<const unsigned short*>(in))); break;
        case Keyword::UNSIGNED_LONG:      out->append(from(*static_cast<const unsigned long*>(in))); break;
        case Keyword::UNSIGNED_LONG_LONG: out->append(from(*static_cast<const unsigned long long*>(in))); break;

        // floating
        case Keyword::FLOAT:       out->append(from<float>(*static_cast<const float*>(in))); break;
        case Keyword::DOUBLE:      out->append(from<double>(*static_cast<const double*>(in))); break;
        case Keyword::LONG_DOUBLE: out->append(from<long double>(*static_cast<const long double*>(in))); break;

        case Keyword::CLASS: out->append(reinterpret_cast<const Object*>(in)->serialize()); break;
        case Keyword::ENUM:
            // out->append(static_cast<const EInterface*>(in)->stringify());
            break;

        case Keyword::STL_DEQUE:  out->append(from(*static_cast<const Container*>(in))); break;
        case Keyword::STD_STRING: out->append(from(*static_cast<const string*>(in))); break;
    }
}

// deserialize
void Codec::decode(void* out, const std::string& in, const Keyword& type) {
    switch(type) {
        case Keyword::UNREGISTERED:
        case Keyword::VOID:         break;

        case Keyword::POINTER: {
            // TODO:
        } break;

        case Keyword::REFERENCE: {
            // TODO:
        } break;

        case Keyword::UNION: {
            // TODO:
        } break;

            // clang-format off
        case Keyword::SIGNED_INT:         *static_cast<int*>(out)                = to<signed int>(in);         break;
        case Keyword::SIGNED_CHAR:        *static_cast<char*>(out)               = to<signed char>(in);        break;
        case Keyword::SIGNED_SHORT:       *static_cast<short*>(out)              = to<signed short>(in);       break;
        case Keyword::SIGNED_LONG:        *static_cast<long*>(out)               = to<signed long>(in);        break;
        case Keyword::SIGNED_LONG_LONG:   *static_cast<signed long long*>(out)   = to<signed long long>(in);   break;
        case Keyword::UNSIGNED_INT:       *static_cast<int*>(out)                = to<unsigned int>(in);       break;
        case Keyword::UNSIGNED_CHAR:      *static_cast<char*>(out)               = to<unsigned char>(in);      break;
        case Keyword::UNSIGNED_SHORT:     *static_cast<short*>(out)              = to<unsigned short>(in);     break;
        case Keyword::UNSIGNED_LONG:      *static_cast<long*>(out)               = to<unsigned long>(in);      break;
        case Keyword::UNSIGNED_LONG_LONG: *static_cast<unsigned long long*>(out) = to<unsigned long long>(in); break;
        case Keyword::CHAR:               *static_cast<char*>(out)               = to<char>(in);               break;
        case Keyword::BOOL:               *static_cast<bool*>(out)               = to<bool>(in);               break;
        case Keyword::FLOAT:              *static_cast<float*>(out)              = to<float>(in);              break;
        case Keyword::DOUBLE:             *static_cast<double*>(out)             = to<double>(in);             break;
        case Keyword::LONG_DOUBLE:        *static_cast<long double*>(out)        = to<long double>(in);        break;
        case Keyword::STD_STRING:         *static_cast<string*>(out)             = to<string>(in);             break;
            // clang-format on

        case Keyword::CLASS: Object::deserialize(static_cast<Object*>(out), in); break;
        case Keyword::ENUM:
            // static_cast<EInterface*>(out)->parse(in);
            break;

        case Keyword::STL_DEQUE: static_cast<Container*>(out)->deserialize(in); break;
    }
}

/**************************************************************************************************
 * parse end position check
 **************************************************************************************************/
template<typename T> bool Codec::parsed(const string& in, size_t idx) {
    // string -> find `\"`
    if constexpr(std::is_same_v<T, string>) {
        if(in[idx] != '\"') return false;
    }

    // container -> find `],`
    else if constexpr(isSTL<T>()) {
        if(in[idx] != ']') return false;
    }

    // object -> find `},`
    else if constexpr(std::is_base_of_v<Object, T> || std::is_same_v<Object, T>) {
        if(in[idx] != '}') return false;
    }

    // primitive types
    else if constexpr(!std::is_same_v<char, T>) {
        if(in[idx] == '\0') return true; // stirng, 0 is '0', \0 is end pos
        if(in[idx] == ',') return true;  // primitive has not escape sequnce
        return false;
    }

    // find `,`
    // <char>   e.g. `'A',` [idx] == `,` -> [idx]
    // <object> e.g. `{A},` [idx] == `}` -> [idx + 1]
    if constexpr(std::is_same_v<char, T>) {
        if(in[idx] != ',') {
            return false; // not parse end
        }
        // check escape character
        if(in[idx - 1] == '\\') {
            if(in[idx - 2] == '\\') {
                return true; // `\\`
            }
            return false; // `*\,`
        }
        return true; // not escape
    }

    else {
        if(in[idx + 1] != ',') {
            return false; // not parse end
        }
        if(in[idx - 1] == '\\') {
            // e.g. "{ "\\}," }, { "" }"
            // safe reason 1. end brace ` }` (with space)
            // safe reason 2. impossible `\\}` (valid: `\\\}` or `\}`)
            return false;
        }
        return true;
    }
}

} // namespace meta

LWE_END

#endif
