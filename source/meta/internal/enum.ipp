#ifdef LWE_META_ENUM

LWE_BEGIN
namespace meta {

template<typename E> const char* Enum::serialize(E in) {
    static std::unordered_map<E, const char*> cache;

    auto result = cache.find(in);
    if(result != cache.end()) {
        return result->second;
    }

    const Enumerate& reflected = Enumerate::find<E>();
    for(auto i : reflected) {
        if(i.value == static_cast<uint64_t>(in)) {
            cache[in] = i.name;
            return i.name;
        }
    }
    return "";
}

const char* Enum::serialize(const string& type, uint64_t value) {
    const Enumerate& reflected = Enumerate::find(type);
    for(auto i : reflected) {
        if(i.value == value) {
            return i.name;
        }
    }
    return "";
}

const char* Enum::serialize(const char* type, uint64_t value) {
    return serialize(string{ type }, value);
}

template<typename E> E Enum::deserialize(const char* in) {
    return deserialize(string{ in });
}

template<typename E> E Enum::deserialize(const string& in) {
    std::unordered_map<string, E> cache;

    auto result = cache.find(in);
    if(result != cache.end()) {
        return result->second;
    }

    const Enumerate& reflected = Enumerate::find<E>();
    for(auto i : reflected) {
        if(i.name == in) {
            cache[in] = static_cast<E>(i.value);
            return static_cast<E>(i.value);
        }
    }
    return static_cast<E>(0);
}

uint64_t Enum::deserialize(const string& type, const string& name) {
    const Enumerate& reflected = Enumerate::find(type);
    for(auto i : reflected) {
        if(i.name == name) {
            return i.value;
        }
    }
    return 0;
}

}
LWE_END
#endif