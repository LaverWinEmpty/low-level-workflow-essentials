#ifndef LWE_UTIL_RANDOM
#define LWE_UTIL_RANDOM

#include "../common.hpp"

LWE_BEGIN
namespace util {

class Random {
public:
    using State = uint64[4];

public:
    Random(uint64 = 0);

public:
    Random* instance(uint64 seed = 0);

public:
    void initialize(uint64);

public:
    int64  sint(int64  = INT64_MAX,  int64  = INT64_MIN);
    uint64 uint(uint64 = UINT64_MAX, uint64 = 0);
    double real(double = 1,          double = 0);

private:
    uint64 xoshiro256() const;
    static uint64 splitmix64(uint64&);

private:
    State state;
};

}
LWE_END

#include "random.ipp"
#endif