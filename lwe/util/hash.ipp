#ifdef LWE_UTIL_HASH

LWE_BEGIN
namespace util {

Hash::Hash(const void* in, size_t n): val(FNV1A64_BASIS) {
    const unsigned char* ptr = static_cast<const unsigned char*>(in);
    for (size_t i = 0; i < n; i++) {
        val ^= ptr[i];
        val *= FNV1A64_PRIME;
    }

    // to stirng, string size 17
    snprintf(str, sizeof(str), "%016llX", val);
}

Hash::Hash(const string& in): Hash(in.c_str(), in.size()) {}

bool Hash::operator==(const Hash& in) const {
    return val == in.val;
}

bool Hash::operator!=(const Hash& in) const {
    return val != in.val;
}

Hash::Hash(const char* in) : Hash(in, std::strlen(in)) {}

template<typename T> Hash::Hash(const T& in): Hash(&in, sizeof(T)) {}

const char* Hash::operator*() const {
    return str;
}

Hash::operator hash_t() const {
    return val;
}

}
LWE_END
#endif