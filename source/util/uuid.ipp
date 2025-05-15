#ifdef LWE_UTIL_UUID

LWE_BEGIN

namespace util {

UUID::UUID() : ptr(new String) {
    hi  = Random::generate<uint64>(-1, 0),
    lo  = Random::generate<uint64>(-1, 0);

    auto str = ptr->str;

    unsigned char* hiptr = reinterpret_cast<unsigned char*>(&hi);
    unsigned char* loptr = reinterpret_cast<unsigned char*>(&lo);

    hiptr[6] = (hiptr[6] & 0x0F) | 0x40; // 0x4~
    loptr[1] = (loptr[1] & 0x3F) | 0x80; // 0x8 | 0x9~ | 0xA~ | 0xB~

    sprintf(str, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        hiptr[0], hiptr[1], hiptr[2], hiptr[3], hiptr[4], hiptr[5], hiptr[6], hiptr[7],
        loptr[0], loptr[1], loptr[2], loptr[3], loptr[4], loptr[5], loptr[6], loptr[7]
    );
}

bool UUID::operator==(const UUID& in) const {
    return hi == in.hi && lo == in.lo;
}

bool UUID::operator!=(const UUID& in) const {
    return hi != in.hi || lo != in.lo;
}

const char* UUID::operator*() const {
    return ptr->str;
}

UUID::operator const char* () const {
    return ptr->str;
}

UUID::operator std::string() const {
    return ptr->str;
}

UUID::operator std::string_view() const {
    return ptr->str;
}

}
LWE_END
#endif