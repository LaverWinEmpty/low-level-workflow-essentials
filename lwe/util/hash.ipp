#include "../mem/block.hpp"

LWE_BEGIN
namespace util {

struct Hash::String {
    String(const Hash* in) {
        // to stirng, without null (fixed size)
        snprintf(buffer, 16, "%016llX", in->val);
        buffer[16] = 0;
    }
    operator const char*() { return buffer; }
private:
    char buffer[17];
};

Hash::Hash(const void* in, size_t n): val(FNV1A64_BASIS) {
    const uint8_t* ptr = static_cast<const uint8_t*>(in);
    for(size_t i = 0; i < n; i++) {
        val ^= ptr[i];
        val *= FNV1A64_PRIME;
    }
}

Hash::Hash(const string& in): Hash(in.c_str(), in.size()) { }

Hash::Hash(const char* in): Hash(in, std::strlen(in)) { }

template<typename T> Hash::Hash(const T& in): val(hashof(in)) { }

bool Hash::operator==(const Hash& in) const {
    return val == in.val;
}

bool Hash::operator!=(const Hash& in) const {
    return val != in.val;
}

auto Hash::operator*() const -> String {
    return stringify();
}

Hash::operator hash_t() const {
    return val;
}

auto Hash::stringify() const -> String {
    return String(this);
}

} // namespace util
LWE_END
