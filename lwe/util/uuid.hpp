#ifndef LWE_UTIL_UUID
#define LWE_UTIL_UUID


#include "random.hpp"
#include "buffer.hpp"

LWE_BEGIN
namespace util {

struct UUID {
    //! string proxy
    struct String;

public:
    UUID();

public:
    bool operator==(const UUID&) const;
    bool operator!=(const UUID&) const;

public:
    operator string() const; //! to string

public:
    String operator*() const; //! get string adapter

public:
    Strign stringify() const;

private:
    uint64_t hi, lo;
};

}
LWE_END

#include "uuid.ipp"
#endif