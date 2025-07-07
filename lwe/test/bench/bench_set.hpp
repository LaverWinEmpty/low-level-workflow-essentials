// Env
// - Version:  C++17
// - Arch:     x64
// - CPU:      Ryzen R7-3700x
// - RAM:      DDR4-3200 / 8GB x 2
// - OS:       Windows 11
// - Compiler: MSVC 2022 v143
// - Option:   Release (/O2)
// - Hash:     FNV1a

// Summary
// - small data  (8 bytes):    significantly faster
// - medium data (512 bytes):  slightly faster (approximately 10%  ~  20%)
// - large data  (4096 bytes): slightly faster (approximately 15%  ~  25%)
// - Note: `clear()` is generally faster, but can be slightly slower in some cases
// Output
/*
    element: 10000000
    size: 8
    std insert 0 ~ 10000000 insert time: 3.371 sec
    lwe insert 0 ~ 10000000 insert time: 1.858 sec
    std loop 0 ~ 10000000 insert time: 0.663 sec ... -2014260032
    lwe loop 0 ~ 10000000 insert time: 0.207 sec ... -2014260032
    std loop clear time: 1.042 sec
    lwe loop clear time: 0.287 sec
    ================================================================================
    element: 1000000
    size: 512
    std insert 0 ~ 1000000 insert time: 1.547 sec
    lwe insert 0 ~ 1000000 insert time: 1.28 sec
    std loop 0 ~ 1000000 insert time: 0.105 sec ... 1783293664
    lwe loop 0 ~ 1000000 insert time: 0.084 sec ... 1783293664
    std loop clear time: 0.145 sec
    lwe loop clear time: 0.076 sec
    ================================================================================
    element: 100000
    size: 4096
    std insert 0 ~ 100000 insert time: 1.087 sec
    lwe insert 0 ~ 100000 insert time: 0.813 sec
    std loop 0 ~ 100000 insert time: 0.031 sec ... 704982704
    lwe loop 0 ~ 100000 insert time: 0.027 sec ... 704982704
    std loop clear time: 0.048 sec
    lwe loop clear time: 0.049 sec
*/

// test code

#include "iostream"

#include "../../stl/set.hpp"
#include "../../util/timer.hpp"

float timer(std::function<void()> fn) {
    LWE::util::Timer t;
    fn();
    return t.sec();
}

char table[255];

template<size_t N>
struct LargeData {
    LargeData(int n = 0): n(n) { memset(buffer, table[n & 0xFF], N - sizeof(int)); } // dummy set
    operator int() { return n; }
    bool operator==(const LargeData& in) const { return n == in.n; }
    bool operator!=(const LargeData& in) const { return n != in.n; }

    int  n;
    char buffer[N - sizeof(int)] = { 0 };
};

template<size_t N>
struct std::hash<LargeData<N>> {
    size_t operator()(const LargeData<N>& in) const { return LWE::util::hashof(in); }
};

using namespace lwe::mem;
using namespace lwe::stl;

int main() {
    for(int i = 0; i < 255; ++i) {
        table[i] = i;
    }

    float sec;
    int   output; // Force read of variable

    static constexpr float LOAD_FACTOR = 0.75;

    /* small data */ {
        using Type                = LargeData<8>;
        static constexpr int LOOP = 10'000'000; // small data
        std::cout << "element: " << LOOP << "\n";

        std::unordered_set<Type> stdset;
        LWE::stl::Set<Type>      lweset(LOAD_FACTOR);

        std::cout << "size: " << sizeof(Type) << "\n";

        sec = timer([&]() {
            for(int i = 0; i < LOOP; ++i) {
                stdset.insert(i);
            }
        });
        std::cout << "std insert 0 ~ " << LOOP << " insert time: " << sec << " sec\n";

        sec = timer([&]() {
            for(int i = 0; i < LOOP; ++i) {
                lweset.push(i);
            }
        });
        std::cout << "lwe insert 0 ~ " << LOOP << " insert time: " << sec << " sec\n";

        output = 0;
        sec    = timer([&]() {
            for(auto itr : stdset) {
                output += itr;
            }
        });
        std::cout << "std loop 0 ~ " << LOOP << " insert time: " << sec << " sec" << " ... " << output << "\n";

        output = 0;
        sec    = timer([&]() {
            for(auto itr : lweset) {
                output += itr;
            }
        });
        std::cout << "lwe loop 0 ~ " << LOOP << " insert time: " << sec << " sec" << " ... " << output << "\n";

        sec = timer([&]() { stdset.clear(); });
        std::cout << "std loop clear time: " << sec << " sec\n";
        sec = timer([&]() { lweset.clear(); });
        std::cout << "lwe loop clear time: " << sec << " sec\n";
    }

    std::cout << "================================================================================\n";

    /* middle data */ {
        using Type                = LargeData<512>;
        static constexpr int LOOP = 1'000'000;
        std::cout << "element: " << LOOP << "\n";

        std::unordered_set<Type> stdset;
        LWE::stl::Set<Type>      lweset(LOAD_FACTOR);

        std::cout << "size: " << sizeof(Type) << "\n";

        sec = timer([&]() {
            for(int i = 0; i < LOOP; ++i) {
                stdset.insert(i);
            }
        });
        std::cout << "std insert 0 ~ " << LOOP << " insert time: " << sec << " sec\n";

        sec = timer([&]() {
            for(int i = 0; i < LOOP; ++i) {
                lweset.push(i);
            }
        });
        std::cout << "lwe insert 0 ~ " << LOOP << " insert time: " << sec << " sec\n";

        output = 0;
        sec    = timer([&]() {
            for(auto itr : stdset) {
                output += itr;
            }
        });
        std::cout << "std loop 0 ~ " << LOOP << " insert time: " << sec << " sec" << " ... " << output << "\n";

        output = 0;
        sec    = timer([&]() {
            for(auto itr : lweset) {
                output += itr;
            }
        });
        std::cout << "lwe loop 0 ~ " << LOOP << " insert time: " << sec << " sec" << " ... " << output << "\n";

        sec = timer([&]() { stdset.clear(); });
        std::cout << "std loop clear time: " << sec << " sec\n";
        sec = timer([&]() { lweset.clear(); });
        std::cout << "lwe loop clear time: " << sec << " sec\n";
    }

    std::cout << "================================================================================\n";

    /* large data */ {
        using Type                = LargeData<4'096>;
        static constexpr int LOOP = 100'000; // large data
        std::cout << "element: " << LOOP << "\n";

        std::unordered_set<Type> stdset;
        LWE::stl::Set<Type>      lweset(LOAD_FACTOR);

        std::cout << "size: " << sizeof(Type) << "\n";

        sec = timer([&]() {
            for(int i = 0; i < LOOP; ++i) {
                stdset.insert(i);
            }
        });
        std::cout << "std insert 0 ~ " << LOOP << " insert time: " << sec << " sec\n";

        sec = timer([&]() {
            for(int i = 0; i < LOOP; ++i) {
                lweset.push(i);
            }
        });
        std::cout << "lwe insert 0 ~ " << LOOP << " insert time: " << sec << " sec\n";

        output = 0;
        sec    = timer([&]() {
            for(auto itr : stdset) {
                output += itr;
            }
        });
        std::cout << "std loop 0 ~ " << LOOP << " insert time: " << sec << " sec" << " ... " << output << "\n";

        output = 0;
        sec    = timer([&]() {
            for(auto itr : lweset) {
                output += itr;
            }
        });
        std::cout << "lwe loop 0 ~ " << LOOP << " insert time: " << sec << " sec" << " ... " << output << "\n";

        sec = timer([&]() { stdset.clear(); });
        std::cout << "std loop clear time: " << sec << " sec\n";
        sec = timer([&]() { lweset.clear(); });
        std::cout << "lwe loop clear time: " << sec << " sec\n";
    }
}
