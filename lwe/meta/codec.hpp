#ifndef LWE_META_CODEC
#define LWE_META_CODEC

#include "iostream"

#include "internal/decoder.hpp"
#include "internal/feature.hpp"
#include "internal/reflector.hpp"
#include "object.hpp"
// #include "../stl/pair.hpp"

template<typename T>
struct TypeError;

LWE_BEGIN
namespace meta {

class Object;

class Codec: Static {
public:
    template<typename T> static String from(const T&);       //!< (to string) from T
    template<typename T> static T      to(const StringView); //!< to T (from string)

public:
    template<typename T> static String encode(const Container&);             //!< container encoder
    template<typename T> static void   decode(Container*, const StringView); //!< container decoder

public:
    static String encode(const Object&);             //!< Object encoder
    static void   decode(Object*, const StringView); //!< Object decoder

public:
    template<typename T, typename U> static String encode(const std::pair<T, U>&);             //!< pair encode
    template<typename T, typename U> static void   decode(std::pair<T, U>*, const StringView); //!< pair decode

private:
    static void encode(String*, const void*, Keyword);    //!< run-time serialize
    static void decode(void*, const StringView, Keyword); //!< run-time deserialize

private:
    template<typename T> static void from(String*, const void*);  // run-time encode helper
    template<typename T> static void to(void*, const StringView); // run-time decode helper

public:
    template<typename T> static const char* map(T);                // enum to string
    template<typename T> static T           map(const StringView); // string to enum

public:
    static const char* map(const StringView, uint64_t);         // enum to string run-time
    static uint64_t    map(const StringView, const StringView); // string to enum run-time
};

/**************************************************************************************************
 * default
 **************************************************************************************************/
// serialize
template<typename T> String Codec::from(const T& in) {
    // serializable types
    if constexpr(std::is_base_of_v<Encoder, T> || std::is_same_v<Encoder, T>) {
        return static_cast<const Encoder&>(in).serialize();
    }

    // template
    else if constexpr(TypeEraser<T>::KEYWORD != Keyword::UNREGISTERED) {
        return encode(in); // pair, container, etc
    }

    // enum TODO
    else if constexpr(std::is_enum_v<T>) { }

    // primitive
    else if constexpr(std::is_arithmetic_v<T>) {
        std::stringstream ss;
        if constexpr(std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) ss << int(in);
        // else if constexpr(std::is_same_v<T, float>) ss << std::fixed << std::setprecision(6) << in;
        // else if constexpr(std::is_same_v<T, double>) ss << std::fixed << std::setprecision(14) << in;
        else ss << in;
        return ss.str();
    }

    // etc call: exception
    throw diag::error(diag::INVALID_DATA);
}

// deserialize
template<typename T> T Codec::to(const StringView in) {
    // serializable types
    if constexpr(std::is_base_of_v<Encoder, T> || std::is_same_v<Encoder, T>) {
        T data;
        static_cast<Encoder&>(data).deserialize(in);
        return std::move(data);
    }

    // template
    else if constexpr(TypeEraser<T>::KEYWORD != Keyword::UNREGISTERED) {
        T data;
        decode(&data, in); // container, pair, etc...
        return data;
    }

    // enum
    else if constexpr(std::is_enum_v<T>) {
        const Enumeration& info = Enumeration::find<T>();
        for(auto i : info) {
            if(i.value == static_cast<uint64_t>(in)) {
                return i.name;
            }
        }
        return "";
    }

    // primitive
    else if constexpr(std::is_arithmetic_v<T>) {
        // string to primitive type
        Decoder decoder(in);
        if(!decoder.next<void>()) {
            throw diag::error(diag::INVALID_DATA);
        }
        const StringView str = decoder.get();

        T result;

        if constexpr(std::is_same_v<int8_t, T> || std::is_same_v<uint8_t, T>) {
            int temp;
            std::from_chars(str.data(), str.data() + str.size(), temp);
            return T(temp);
        }

        else {
            std::from_chars(str.data(), str.data() + str.size(), result);
            return result;
        }
    }

    // etc call: exception
    throw diag::error(diag::INVALID_DATA);
}

/**************************************************************************************************
 * character specialization
 **************************************************************************************************/
// serialize
template<> String Codec::from<char>(const char& in) {
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
        default:   return String(1, in);
    }
}

// deserialize
template<> char Codec::to<char>(const StringView in) {
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
template<> String Codec::from<bool>(const bool& in) {
    return in ? "true" : "false";
}

// deserialize
template<> bool Codec::to<bool>(const StringView in) {
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
template<> String Codec::from<String>(const String& in) {
    String out;
    out.append("\"");
    size_t loop = in.size();
    for(size_t i = 0; i < loop; ++i) {
        out.append(from<char>(in[i]));
    }
    out.append("\"");
    return out;
}

// deserialize
template<> String Codec::to<String>(const StringView in) {
    String result;
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
template<typename T> String Codec::encode(const Container& in) {
    auto curr = reinterpret_cast<const T&>(in).begin();
    auto last = reinterpret_cast<const T&>(in).end();

    std::string out;

    // has data
    if(curr != last) {
        out.append("[ ");
        // for each
        while(true) {
            out.append(from<typename T::value_type>(*curr)); // auto
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
template<typename T> void Codec::decode(Container* ptr, StringView in) {
    if(in == "[]") {
        return; // empty
    }
    T& out = *reinterpret_cast<T*>(ptr); // else

    Decoder decoder(in);
    decoder.move(2); // ignore `[ `
    while(true) {
        decoder.next<typename T::value_type>();

        // deserialize
        typename T::value_type data;
        data = to<typename T::value_type>(decoder.get()); // auto
        out.push(std::move(data));

        // move 2 reason
        // ing -> `, `
        // end -> ` ]`
        decoder.move(2);

        // check end
        if(!decoder.check<Container>()) {
            break;
        }
    }
}

/**************************************************************************************************
 * object deail
 **************************************************************************************************/
String Codec::encode(const Object& in) {
    const Structure& prop = in.meta()->fields();
    if(prop.size() == 0) {
        return "{}";
    }

    String buffer;
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
void Codec::decode(Object* out, StringView in) {
    // empty
    if(in == "{}") {
        return;
    }

    const Structure& prop = out->meta()->fields();
    if(prop.size() == 0) {
        throw diag::error(diag::INVALID_DATA);
    }

    char* ptr = const_cast<char*>(reinterpret_cast<const char*>(out));

    Decoder decoder(in);
    decoder.move(2); // ignore `{ `

    size_t loop = prop.size() - 1;
    for(int i = 0; i < loop; ++i) {
        decoder.next(prop[i].type); // find next
        decode(ptr + prop[i].offset, decoder.get(), prop[i].type.code());
        decoder.move(2); // ignore `, `
    }

    // last
    decoder.next(prop[loop].type); // find next
    decoder.trim(-2);              // ignore ` }`
    decode(ptr + prop[loop].offset, decoder.get(), prop[loop].type.code());
}

// definition
String Object::serialize() const {
    return Codec::encode(*this);
}
void Object::deserialize(const StringView in) {
    Codec::decode(this, in);
}
// void Object::deserialize(Object* out, const StringView in) {
//    out->deserialize(in);
//}

/**************************************************************************************************
 * pair
 **************************************************************************************************/
// serialize
template<typename K, typename V> String Codec::encode(const std::pair<K, V>& in) {
    String out;
    out += "{ ";
    out += from<K>(in.first);
    out += ", ";
    out += from<V>(in.second);
    out += " }";
    return out;
}

// deserialize
template<typename K, typename V> void Codec::decode(std::pair<K, V>* out, StringView in) {
    Decoder decoder(in);

    decoder.move(2); // ignore `{ `
    if(!decoder.next<K>()) {
        throw diag::error(diag::INVALID_DATA);
    }
    out->first = to<K>(decoder.get());

    decoder.move(2); // ignore `, `
    if(!decoder.next<V>()) {
        throw diag::error(diag::INVALID_DATA);
    }
    out->second = to<V>(decoder.get());
}

/**************************************************************************************************
 * enum detail
 **************************************************************************************************/
// serialize
template<typename T> static const char* Codec::map(T in) {
    const Enumeration& reflected = Enumeration::find<T>();
    for(auto i : reflected) {
        if(i.value == static_cast<uint64_t>(in)) {
            return i.name;
        }
    }
    return "";
}

// serialize run-time
const char* Codec::map(const StringView type, uint64_t in) {
    const Enumeration& reflected = Enumeration::find(type.data());
    for(auto i : reflected) {
        if(i.value == static_cast<uint64_t>(in)) {
            return i.name;
        }
    }
    return "";
}

// deserialize
template<typename T> T Codec::map(const StringView in) {
    const Enumeration& reflected = Enumeration::find<T>();
    for(auto i : reflected) {
        if(i.name == in) {
            return static_cast<T>(i.value);
        }
    }
    return static_cast<T>(0);
}

// deserialize run-time
uint64_t Codec::map(const StringView type, const StringView in) {
    const Enumeration& reflected = Enumeration::find(type.data());
    for(auto i : reflected) {
        if(i.name == in) {
            return i.value;
        }
    }
    return 0;
}

// enum defeinition
template<typename E> const char* Enum::serialize(E in) {
    return Codec::map<E>(in);
}
const char* Enum::serialize(const StringView type, uint64_t value) {
    return Codec::map(type, value);
}
template<typename E> E Enum::deserialize(const StringView value) {
    return Codec::map<E>(value);
}
uint64_t Enum::deserialize(const StringView type, const StringView value) {
    return Codec::map(type, value);
}

/**************************************************************************************************
 * run-time serialization
 **************************************************************************************************/
// helper
template<typename T> void Codec::to(void* out, const StringView in) {
    *static_cast<T*>(out) = to<T>(in);
}
template<typename T> void Codec::from(String* out, const void* in) {
    out->append(from<T>(*static_cast<const T*>(in)));
}

// serialize
void Codec::encode(std::string* out, const void* in, Keyword type) {
    if(type == Keyword::STD_PAIR) {
        // RUNTIME SERIALIZE NOT SUPPORTED TYPE
        throw diag::error(diag::INVALID_DATA);
    }

    switch(type) {
            // clang-format off
        case Keyword::BOOL:               from<bool>(out, in);               break;
        case Keyword::CHAR:               from<char>(out, in);               break;
        case Keyword::SIGNED_INT:         from<signed int>(out, in);         break;
        case Keyword::SIGNED_SHORT:       from<signed short>(out, in);       break;
        case Keyword::SIGNED_LONG:        from<signed long>(out, in);        break;
        case Keyword::SIGNED_CHAR:        from<signed char>(out, in);        break;
        case Keyword::SIGNED_LONG_LONG:   from<signed long long>(out, in);   break;
        case Keyword::UNSIGNED_INT:       from<unsigned int>(out, in);       break;
        case Keyword::UNSIGNED_CHAR:      from<unsigned char>(out, in);      break;
        case Keyword::UNSIGNED_SHORT:     from<unsigned short>(out, in);     break;
        case Keyword::UNSIGNED_LONG:      from<unsigned long>(out, in);      break;
        case Keyword::UNSIGNED_LONG_LONG: from<unsigned long long>(out, in); break;
        case Keyword::FLOAT:              from<float>(out, in);              break;
        case Keyword::DOUBLE:             from<double>(out, in);             break;
        case Keyword::LONG_DOUBLE:        from<long double>(out, in);        break;
            // clang-format on

        case Keyword::CLASS: out->append(reinterpret_cast<const Object*>(in)->serialize()); break;
        case Keyword::ENUM:
            // out->append(static_cast<const EInterface*>(in)->stringify());
            break;

        case Keyword::STD_STRING: from<String>(out, in); break;
        case Keyword::STL_STACK:  from<Encoder>(out, in); break;
        case Keyword::STL_DEQUE:  from<Encoder>(out, in); break;
        case Keyword::STL_MAP:    from<Encoder>(out, in); break;
        case Keyword::STL_SET:    from<Encoder>(out, in); break;

        case Keyword::UNREGISTERED:
        case Keyword::VOID:         break;
        case Keyword::POINTER:      break;
        case Keyword::REFERENCE:    break;
        case Keyword::UNION:        break;
    }
}

// deserialize
void Codec::decode(void* out, const StringView in, Keyword type) {
    if(type == Keyword::STD_PAIR) {
        // RUNTIME SERIALIZE NOT SUPPORTED TYPE
        throw diag::error(diag::INVALID_DATA);
    }

    switch(type) {
            // clang-format off
        case Keyword::SIGNED_INT:         to<signed int>(out, in);         break;
        case Keyword::SIGNED_CHAR:        to<signed char>(out, in);        break;
        case Keyword::SIGNED_SHORT:       to<signed short>(out, in);       break;
        case Keyword::SIGNED_LONG:        to<signed long>(out, in);        break;
        case Keyword::SIGNED_LONG_LONG:   to<signed long long>(out, in);   break;
        case Keyword::UNSIGNED_INT:       to<unsigned int>(out, in);       break;
        case Keyword::UNSIGNED_CHAR:      to<unsigned char>(out, in);      break;
        case Keyword::UNSIGNED_SHORT:     to<unsigned short>(out, in);     break;
        case Keyword::UNSIGNED_LONG:      to<unsigned long>(out, in);      break;
        case Keyword::UNSIGNED_LONG_LONG: to<unsigned long long>(out, in); break;
        case Keyword::CHAR:               to<char>(out, in);               break;
        case Keyword::BOOL:               to<bool>(out, in);               break;
        case Keyword::FLOAT:              to<float>(out, in);              break;
        case Keyword::DOUBLE:             to<double>(out, in);             break;
        case Keyword::LONG_DOUBLE:        to<long double>(out, in);        break;
            // clang-format on

        case Keyword::CLASS: static_cast<Object*>(out)->deserialize(in); break;
        case Keyword::ENUM:
            // static_cast<EInterface*>(out)->parse(in);
            break;

        case Keyword::STD_STRING: *static_cast<String*>(out) = to<String>(in); break;
        case Keyword::STL_STACK:  static_cast<Encoder*>(out)->deserialize(in); break;
        case Keyword::STL_DEQUE:  static_cast<Encoder*>(out)->deserialize(in); break;
        case Keyword::STL_MAP:    static_cast<Encoder*>(out)->deserialize(in); break;
        case Keyword::STL_SET:    static_cast<Encoder*>(out)->deserialize(in); break;

        case Keyword::UNREGISTERED:
        case Keyword::VOID:         break;
        case Keyword::POINTER:      break;
        case Keyword::REFERENCE:    break;
        case Keyword::UNION:        break;
    }
}

} // namespace meta

/**************************************************************************************************
 * STL definition
 **************************************************************************************************/
//// pair serialize definition
// namespace stl {
// template<typename Key, typename Value> String Pair<Key, Value>::serialize() const {
//    return meta::Codec::encode<Key, Value>(*this);
//}
// template<typename Key, typename Value> void Pair<Key, Value>::deserialize(const StringView in) {
//    meta::Codec::decode<Key, Value>(this, in);
//}
//} // namespace stl
LWE_END
#endif
