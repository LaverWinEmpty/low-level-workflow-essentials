#ifndef LWE_META_CODEC
#define LWE_META_CODEC

#include "internal/decoder.hpp"
#include "internal/feature.hpp"
#include "internal/reflector.hpp"
#include "object.hpp"
#include "../base/base.h"
#include "../stl/pair.hpp"
#include "../stl/internal/iterator.hpp"
#include <type_traits>

// #include "../mem/ptr.hpp"

LWE_BEGIN
namespace meta {

class Object;

class Codec: Static {
public:
    template<typename T> static string from(const T&);        //!< (to string) from T
    template<typename T> static T      to(const string_view); //!< to T (from string)

public:
    template<typename T> static string encode(const Container&);              //!< container encoder
    template<typename T> static void   decode(Container*, const string_view); //!< container decoder

public:
    static string encode(const Object&);              //!< Object encoder
    static void   decode(Object*, const string_view); //!< Object decoder

public:
    template<typename T, typename U> static string encode(const Pair&);              //!< pair encode
    template<typename T, typename U> static void   decode(Pair*, const string_view); //!< pair decode

private:
    static void encode(string*, const void*, Keyword);     //!< run-time serialize
    static void decode(void*, const string_view, Keyword); //!< run-time deserialize

private:
    template<typename T> static void from(string*, const void*);   // run-time encode helper
    template<typename T> static void to(void*, const string_view); // run-time decode helper

public:
    template<typename T> static const char* map(T);                 // enum to string
    template<typename T> static T           map(const string_view); // string to enum

public:
    static const char* map(const string_view, uint64_t);          // enum to string run-time
    static uint64_t    map(const string_view, const string_view); // string to enum run-time
};

/**************************************************************************************************
 * default
 **************************************************************************************************/
// serialize
template<typename T> string Codec::from(const T& in) {
    // serializable types
    if constexpr(isSTL<T>() || isOBJ<T>() || isPAIR<T>()) {
        return in.serialize();
    }
    else {
        std::stringstream ss;
        if constexpr(std::is_same_v<T, int8_t> || std::is_same_v<T, uint8_t>) ss << int(in);
        // else if constexpr(std::is_same_v<T, float>) ss << std::fixed << std::setprecision(6) << in;
        // else if constexpr(std::is_same_v<T, double>) ss << std::fixed << std::setprecision(14) << in;
        else ss << in;
        return ss.str();
    }
}

// deserialize
template<typename T> T Codec::to(const string_view in) {
    // object or container
    if constexpr(isSTL<T>() || isOBJ<T>() || isPAIR<T>()) {
        T data;
        data.deserialize(in);
        return std::move(data);
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

    // TODO: exeption
    // 1. out of range
    // 2. format mismatch
    // 3. type mismatch ?
    else {
        // string to primitive type
        Decoder decoder(in);
        if(!decoder.next<void>()) {
            throw diag::error(diag::INVALID_DATA);
        }
        const string_view str = decoder.get();

        T result;
        if constexpr(std::is_same_v<int8_t, T> || std::is_same_v<uint8_t, T>) {
            int temp;
            std::from_chars(str.data(), str.data() + str.size(), temp);
            return temp;
        }
        else std::from_chars(str.data(), str.data() + str.size(), result);
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
template<> char Codec::to<char>(const string_view in) {
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
template<> bool Codec::to<bool>(const string_view in) {
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
template<> string Codec::to<string>(const string_view in) {
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
    stl::Iterator<stl::FWD | stl::VIEW, T> curr = reinterpret_cast<const T&>(in).begin();
    stl::Iterator<stl::FWD | stl::VIEW, T> last = reinterpret_cast<const T&>(in).end();

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
template<typename Derived> void Codec::decode(Container* ptr, string_view in) {
    using Element = typename Derived::value_type;
    if(in == "[]") {
        return; // empty
    }
    Derived& out = *reinterpret_cast<Derived*>(ptr); // else

    Decoder decoder(in);
    decoder.move(2); // ignore `[ `
    while(true) {
        decoder.next<Element>();

        // serialize
        Element data;
        decode(reinterpret_cast<void*>(&data), decoder.get(), typecode<Element>());
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
void Codec::decode(Object* out, string_view in) {
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
string Object::serialize() const {
    return Codec::encode(*this);
}
void Object::deserialize(const string_view in) {
    Codec::decode(this, in);
}
void Object::deserialize(Object* out, const string_view in) {
    out->deserialize(in);
}

/**************************************************************************************************
 * pair
 **************************************************************************************************/
// serialize
template<typename K, typename V> string Codec::encode(const Pair& in) {
    const stl::Pair<K, V>& pair = reinterpret_cast<const stl::Pair<K, V>&>(in);

    string out;
    out += "{ ";
    out += from<K>(pair.first());
    out += ", ";
    out += from<V>(pair.second());
    out += " }";

    return out;
}

// deserialize
template<typename K, typename V> void Codec::decode(Pair* out, string_view in) {
    Decoder decoder(in);

    stl::Pair<K, V>* derived = reinterpret_cast<stl::Pair<K, V>*>(out); // casting

    decoder.move(2); // ignore `{ `
    if(!decoder.next<K>()) {
        throw diag::error(diag::INVALID_DATA);
    }
    derived->first() = to<K>(decoder.get());

    decoder.move(2); // ignore `, `
    if(!decoder.next<V>()) {
        throw diag::error(diag::INVALID_DATA);
    }
    derived->second() = to<V>(decoder.get());
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
const char* Codec::map(const string_view type, uint64_t in) {
    const Enumeration& reflected = Enumeration::find(type.data());
    for(auto i : reflected) {
        if(i.value == static_cast<uint64_t>(in)) {
            return i.name;
        }
    }
    return "";
}

// deserialize
template<typename T> T Codec::map(const string_view in) {
    const Enumeration& reflected = Enumeration::find<T>();
    for(auto i : reflected) {
        if(i.name == in) {
            return static_cast<T>(i.value);
        }
    }
    return static_cast<T>(0);
}

// deserialize run-time
uint64_t Codec::map(const string_view type, const string_view in) {
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
const char* Enum::serialize(const string_view type, uint64_t value) {
    return Codec::map(type, value);
}
template<typename E> E Enum::deserialize(const string_view value) {
    return Codec::map<E>(value);
}
uint64_t Enum::deserialize(const string_view type, const string_view value) {
    return Codec::map(type, value);
}

/**************************************************************************************************
 * run-time serialization
 **************************************************************************************************/
// helper
template<typename T> void Codec::to(void* out, const string_view in) {
    *static_cast<T*>(out) = to<T>(in);
}
template<typename T> void Codec::from(string* out, const void* in) {
    out->append(from<T>(*static_cast<const T*>(in)));
}

// serialize
void Codec::encode(std::string* out, const void* in, Keyword type) {
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

        case Keyword::STD_STRING: from<string>(out, in); break;
        case Keyword::STL_PAIR:   from<Pair>(out, in); break;
        case Keyword::STL_DEQUE:  from<Container>(out, in); break;
    }
}

// deserialize
void Codec::decode(void* out, const string_view in, Keyword type) {
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

        case Keyword::CLASS: Object::deserialize(static_cast<Object*>(out), in); break;
        case Keyword::ENUM:
            // static_cast<EInterface*>(out)->parse(in);
            break;

        case Keyword::STD_STRING: *static_cast<string*>(out) = to<string>(in); break;
        case Keyword::STL_PAIR:   static_cast<Pair*>(out)->deserialize(in); break;
        case Keyword::STL_DEQUE:  static_cast<Container*>(out)->deserialize(in); break;
    }
}

} // namespace meta

/**************************************************************************************************
 * STL definition
 **************************************************************************************************/
// pair serialize definition
namespace stl {
template<typename Key, typename Value> string Pair<Key, Value>::serialize() const {
    return meta::Codec::encode<Key, Value>(*this);
}
template<typename Key, typename Value> void Pair<Key, Value>::deserialize(const string_view in) {
    meta::Codec::decode<Key, Value>(this, in);
}

} // namespace stl
LWE_END
#endif
