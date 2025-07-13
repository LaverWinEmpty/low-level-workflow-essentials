LWE_BEGIN
namespace util {

Hash<void>::Hash(hash_t in): value(in) { }

template<typename T> Hash<void>::Hash(const T& in) {
    if constexpr(std::is_same_v<String, T> || std::is_same_v<StringView, T>) {
        value = fnv1a(in.data(), in.size()); // string by fnv1a
    }
    else if constexpr(std::is_convertible_v<T, const char*>) {
        if(in == nullptr) throw diag::error(diag::INVALID_DATA);
        value = fnv1a(in, std::strlen(in)); // raw string by fnv1a
    }
    else if constexpr(std::is_pointer_v<T>) {
        value = reinterpret_cast<uintptr_t>(in); // pointer to int
    }
    else if constexpr(std::is_integral_v<T>) {
        value = hash_t(in); // return as is
    }
    else if constexpr(std::is_floating_point_v<T>) {
        value = in == -0 ? 0 : in;
        value = fnv1a(&value, sizeof(T));
    }
    else {
        char buffer[sizeof(T)] = { 0 };     // padding space value to 0
        new(buffer) T(in);                  // construct: copy fields
        value = fnv1a(buffer, sizeof(T));   // hash
        reinterpret_cast<T*>(buffer)->~T(); // deconstruct
    }
}

Hash<void>::operator hash_t() const {
    return value;
}

auto Hash<void>::operator*() const -> StringProxy {
    StringProxy out;
    snprintf(out.data(), 17, "%016llX", value);
    return out;
}

hash_t Hash<void>::fnv1a(const void* in, size_t n) {
    static constexpr uint64_t FNV1A64_BASIS = 14'695'981'039'346'656'037ULL;
    static constexpr uint64_t FNV1A64_PRIME = 1'099'511'628'211ULL;

    hash_t         value = FNV1A64_BASIS;
    const uint8_t* ptr   = reinterpret_cast<const uint8_t*>(in);
    for(size_t i = 0; i < n; i++) {
        value ^= ptr[i];
        value *= FNV1A64_PRIME;
    }
    return value;
}

} // namespace util
LWE_END
