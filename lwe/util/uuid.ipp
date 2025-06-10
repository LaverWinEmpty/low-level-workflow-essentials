#ifdef LWE_UTIL_UUID

#include "../mem/allocator.hpp"
#include "../async/lock.hpp"

LWE_BEGIN

namespace util {

struct UUID::String {
    String(const UUID* self) {
        // get
        unsigned const char* hiptr = reinterpret_cast<unsigned const char*>(&self->hi);
        unsigned const char* loptr = reinterpret_cast<unsigned const char*>(&self->lo);
        // copy: without null (fixed size)
        snprintf(buffer, 36,
            "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            hiptr[0], hiptr[1], hiptr[2], hiptr[3],
            hiptr[4], hiptr[5],
            hiptr[6], hiptr[7],
            loptr[0], loptr[1],
            loptr[2], loptr[3], loptr[4], loptr[5], loptr[6], loptr[7]
        );
        buffer[36] = 0;
    }
    operator string()       { return buffer; }
    operator const char* () { return buffer; }
private:
    char buffer[37];
};

UUID::UUID() {
    hi  = Random::generate<uint64_t>(-1, 0),
    lo  = Random::generate<uint64_t>(-1, 0);

    unsigned char* hiptr = reinterpret_cast<unsigned char*>(&hi);
    unsigned char* loptr = reinterpret_cast<unsigned char*>(&lo);

    hiptr[6] = (hiptr[6] & 0x0F) | 0x40; // 0x4~
    loptr[1] = (loptr[1] & 0x3F) | 0x80; // 0x8 | 0x9~ | 0xA~ | 0xB~
}

bool UUID::operator==(const UUID& in) const {
    return hi == in.hi && lo == in.lo;
}

bool UUID::operator!=(const UUID& in) const {
    return hi != in.hi || lo != in.lo;
}

UUID::operator string() const {
    return operator*();
}

auto UUID::operator* () const -> String {
    return String(this);
}

}
LWE_END
#endif