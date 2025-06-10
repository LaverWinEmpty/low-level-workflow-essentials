#ifdef LWE_UTIL_HASH

LWE_BEGIN
namespace util {
thread_local char Hash::buffer[] = { 0 };

Hash::Hash(const void* in, size_t n): val(FNV1A64_BASIS) {
    const unsigned char* ptr = static_cast<const unsigned char*>(in);
    for (size_t i = 0; i < n; i++) {
        val ^= ptr[i];
        val *= FNV1A64_PRIME;
    }
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

Hash::operator string () const {
    // to stirng, without null (fixed size)
    snprintf(buffer, sizeof(buffer) - 1, "%016llX", val);
    return buffer;
}

Hash::operator hash_t() const {
    return val;
}

}
LWE_END
#endif