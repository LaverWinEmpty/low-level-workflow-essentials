#ifndef LWE_UTIL_UUID
#define LWE_UTIL_UUID


#include "random.hpp"

LWE_BEGIN
namespace util {

struct UUID {
    struct String {
        char str[37];
    };

public:
    UUID();

public:
    bool operator==(const UUID&) const;
    bool operator!=(const UUID&) const;

public:
    const char* operator*() const;
    operator const char* () const;
    operator std::string() const;
    operator std::string_view() const;

private:
    uint64_t hi, lo;
    std::shared_ptr<String> ptr;
};

}
LWE_END

#include "uuid.ipp"
#endif