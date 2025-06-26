LWE_BEGIN
namespace util {

Random::Random(uint64_t seed) {
    state[0] = splitmix64(seed);
    state[1] = splitmix64(seed);
    state[2] = splitmix64(seed);
    state[3] = splitmix64(seed);
}

Random* Random::instance(uint64_t seed) {
    static thread_local Random random(seed ? seed : time(nullptr));
    return &random;
}

void Random::initialize(uint64_t seed) {
    state[0] = splitmix64(seed);
    state[1] = splitmix64(seed);
    state[2] = splitmix64(seed);
    state[3] = splitmix64(seed);
}

uint64_t Random::xoshiro256() {
    auto ro64 = [](uint64_t in, size_t rot) {
        return (in << rot | in >> rot);
    };

    uint64_t result = ro64(state[1] * 5, 7) * 9;
    uint64_t temp   = state[1] << 17;

    state[2] ^= state[0];
    state[3] ^= state[1];
    state[1] ^= state[2];
    state[0] ^= state[3];
    state[2] ^= temp;
    state[3]  = ro64(state[3], 45);

    return result;
}

uint64_t Random::splitmix64(uint64_t& inout) {
    uint64_t result = inout += 0x9E'37'79'B9'7F'4A'7C'15;
    result                   = (result ^ (result >> 30)) * 0xBF'58'47'6D'1C'E4'E5'B9;
    result                   = (result ^ (result >> 27)) * 0x94'D0'49'BB'13'31'11'EB;
    return result ^ (result >> 31);
}

int64_t Random::sint(int64_t a, int64_t b) {
    return int64_t(uint(a, b));
}

uint64_t Random::uint(uint64_t a, uint64_t b) {
    // check
    if(a == b) {
        return uint64_t(a); // same, one value
    }

    // cast
    uint64_t umin = uint64_t(a);
    uint64_t umax = uint64_t(b) + 1;
    if(umin == umax) {
        return xoshiro256(); // overflowed, all values
    }

    // swap
    if(a > b) {
        uint64_t temp = umin;
        umin          = umax;
        umax          = temp;
    }

    // set
    uint64_t range = umax - umin;
    uint64_t mask  = ~uint64_t(0);
    uint64_t limit = mask - mask % range;

    // rejection sampling
    uint64_t result = xoshiro256();
    while(result >= limit) {
        result = xoshiro256();
    }
    return uint64_t(umin + result % range);
}

double Random::real(double a, double b) {
    // no calculate case
    if(a == b) {
        return static_cast<double>(a);
    }

    // swap
    if(a > b) {
        double temp = a;
        a           = b;
        b           = temp;
    }

    static uint64_t ONE = 1ULL << 52; // IEEE754 exponent one bit

    uint64_t mantissa = xoshiro256() & ((ONE - 1)); // get mantissa of random

    double result = double(mantissa) / ONE; // nomalization

    // return
    return a + result * (b - a);
}

} // namespace util
LWE_END
