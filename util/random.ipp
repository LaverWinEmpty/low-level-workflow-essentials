#ifdef LWE_UTIL_RANDOM

LWE_BEGIN
namespace util {

Random::Random(uint64 seed) {
    state[0] = splitmix64(seed);
    state[1] = splitmix64(seed);
    state[2] = splitmix64(seed);
    state[3] = splitmix64(seed);
}

Random* Random::instance(uint64 seed) {
    static thread_local Random random(seed ? seed : time(nullptr));
    return &random;
}

void Random::initialize(uint64 seed) {
    state[0] = splitmix64(seed);
    state[1] = splitmix64(seed);
    state[2] = splitmix64(seed);
    state[3] = splitmix64(seed);
}

uint64 Random::xoshiro256() const {
    auto ro64 = [](uint64 in, size_t rot) {
        return (in << rot | in >> rot);
    };

    uint64 result = ro64(state[1] * 5, 7) * 9;
    uint64 temp   = state[1] << 17;

    state[2] ^= state[0];
    state[3] ^= state[1];
    state[1] ^= state[2];
    state[0] ^= state[3];
    state[2] ^= temp;
    state[3]  = ro64(state[3], 45);

    return result;
}

uint64 Random::splitmix64(uint64& inout) {
    uint64 result = inout += 0x9E3779B97F4A7C15;
    result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
    result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
    return result ^ (result >> 31);
}

int64 Random::sint(int64 a, int64 b) const {
    return int64(uint(a, b));
}

uint64 Random::uint(uint64 a, uint64 b) const {
    // check
    if (a == b) {
        return uint64(a); // same, one value
    }

    // cast
    uint64 umin = uint64(a);
    uint64 umax = uint64(b) + 1;
    if (umin == umax) {
        return xoshiro256(); // overflowed, all values
    }

    // swap
    if (a > b) {
        uint64 temp = umin;
        umin = umax;
        umax = temp;
    }

    // set
    uint64 range = umax - umin;
    uint64 mask  = ~uint64(0);
    uint64 limit = mask - mask % range;

    // rejection sampling 
    uint64 result = xoshiro256();
    while (result >= limit) {
        result = xoshiro256();
    }
    return uint64(umin + result % range);
}

double Random::real(double a, double b) const {
    // no calculate case
    if (a == b) {
        return static_cast<double>(a);
    }

    // swap
    if (a > b) {
        uint64 temp = a;
        a = b;
        b = temp;
    }

    static uint64 ONE = 1ULL << 52; // IEEE754 exponent one bit

    uint64 mantissa = xoshiro256() & ((ONE - 1)); // get mantissa of random

    double result = double(mantissa) / ONE; // nomalization

    // return
    return a + result * (b - a);
}

}
LWE_END
#endif