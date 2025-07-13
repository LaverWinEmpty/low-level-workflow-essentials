#ifndef LWE_UTIL_RANDOM
#define LWE_UTIL_RANDOM

#include "../base/base.h"

LWE_BEGIN
namespace util {

class Random {
public:
    using State = uint64_t[4];

public:
    Random(uint64_t = 0);

public:
    static Random* instance(uint64_t seed = 0);

public:
    void initialize(uint64_t);

public:
    int64_t  sint(int64_t = INT64_MAX, int64_t = INT64_MIN);
    uint64_t uint(uint64_t = UINT64_MAX, uint64_t = 0);
    double   real(double = 1, double = 0);

private:
    uint64_t        xoshiro256();
    static uint64_t splitmix64(uint64_t&);

public:
    template<typename T> static T generate(T a, T b) {
        static Random rand;

        if constexpr(std::is_floating_point_v<T>) {
            return static_cast<T>(rand.real(a, b));
        }
        else if constexpr(std::is_signed_v<T>) {
            return static_cast<T>(rand.sint(a, b));
        }
        else {
            return static_cast<T>(rand.uint(a, b));
        }
    }

private:
    State state;
};

} // namespace util
LWE_END

#include "random.ipp"
#endif
