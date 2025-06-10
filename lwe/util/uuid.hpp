#ifndef LWE_UTIL_UUID
#define LWE_UTIL_UUID


#include "random.hpp"
#include "buffer.hpp"

LWE_BEGIN
namespace util {

struct UUID {
public:
    UUID();

public:
    bool operator==(const UUID&) const;
    bool operator!=(const UUID&) const;

public:
    operator std::string() const;

private:
    uint64_t hi, lo;

private:
    static thread_local char buffer[37];
};

}
LWE_END

#include "uuid.ipp"
#endif