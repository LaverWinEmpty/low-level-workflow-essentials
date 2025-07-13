#define SMALL  1
#define MEDIUM 1
#define LARGE  1

/**************************************************************************************************
 * include
 **************************************************************************************************/
#include <unordered_set>
#include "internal/bench.hpp"
#include "../../stl/set.hpp"
#include "../../util/timer.hpp"
#include "../../util/random.hpp"

using namespace lwe::mem;
using namespace lwe::stl;

/**************************************************************************************************
 * SETTING
 **************************************************************************************************/
static constexpr float LOAD_FACTOR = 1.f; // LWE load factor

char table[255]; // dummy, for fill buffer

template<size_t N> struct Data {
    static const size_t BUFFER_SIZE = N - sizeof(int);

    Data(int n = 0): n(n) { memset(buffer, table[uint8_t(n & 0xFF)], BUFFER_SIZE); } // dummy set
    operator int() { return n; }
    bool operator==(const Data& in) const { return n == in.n && !memcmp(buffer, in.buffer, BUFFER_SIZE); }
    bool operator!=(const Data& in) const { return !(*this == in); }

    int  n;
    char buffer[BUFFER_SIZE] = { 0 };
};

template<size_t N> struct InvalidData: Data<N> {
    using Data<N>::Data;
};

// regist hash to default
namespace lwe::util {
    template<size_t N> Hash(Data<N>) -> Hash<void>;
}

// hash specialize
template<size_t N> struct std::hash<Data<N>> {
    size_t operator()(const Data<N>& in) const { return lwe::util::Hash(in); }
};

// hash sepcialize
template<size_t N> struct lwe::util::Hash<InvalidData<N>>: lwe::util::Hash<void> {
    Hash(const InvalidData<N>&) { value = 100; }
};

// hash specialize
template<size_t N> struct std::hash<InvalidData<N>> {
    size_t operator()(const InvalidData<N>& in) const { return 200; }
};



/**************************************************************************************************
 * main
 **************************************************************************************************/
template<size_t, int> void test_insert();
template<size_t, int> void test_collision();
template<size_t, int> void test_main(); 

int main() {
    // hash check
    InvalidData<100> tester;
    if(lwe::util::Hash<InvalidData<100>>(tester) != 100 || std::hash<InvalidData<100>>{}(tester) != 200) {
        std::cerr << "HASH ERROR\n";
        return -1;  
    }
    // init for dummy data
    for(int i = 0; i < 255; ++i) table[i] = i;

#if SMALL
    test_main<8, 16'000'000>(); // small size data
#endif

#if MEDIUM
    test_main<512, 1'000'000>(); // medium size test
#endif

#if LARGE
    test_main<4'096, 125'000>(); // large size test
#endif
}

template<size_t N, int INSERT> void test_main() {
    std::cout << "Version:  C++17\n"
                 "Arch:     x64\n"
                 "CPU:      Ryzen R7-3700x\n"
                 "RAM:      DDR4-3200 / 8GB x 2\n"
                 "OS:       Windows 11\n"
                 "Compiler: MSVC 2022 v143\n"
                 "Option:   ";

#ifdef NDEBUG
    std::cout << "Release (/O2)\n\n";
#else
    std::cout << "Debug\n\n";
#endif


    auto seed = time(nullptr);

    // time is seed
    std::cout << "TIME:        " << std::string(lwe::util::Timer::system()) + " UTC+09:00\n";
    std::cout << "HASHING:     FNV1a\n";
    std::cout << "RANDOM:      Xoshiro256++ (SEED: " << seed << ")\n";
    std::cout << "LOAD FACTOR: " << LOAD_FACTOR << "\n";
    std::cout << std::endl;

    lwe::util::Random::instance(seed);
    Bench b;

    std::cout << "HASH INSERT / ITERATE / CLEAR TEST\n"; // summary
    b.line();                                            // line
    test_insert<N, INSERT>();                            // insert test
    std::cout << "HASH COLLISION TEST\n";                // summary
    b.line();                                            // line
    test_collision<N, INSERT>();                         // collision test
    std::cout << std::endl;                              // endline
}


/**************************************************************************************************
 * insert test code
 **************************************************************************************************/
template<size_t SIZE, int COUNT> void test_insert() {
    static constexpr int FIND = 1'000'000; // find count

    float        sec = 0;
    float        stlsec;
    volatile int var = 0;

    using Type = Data<SIZE>;

    std::cout << "ELEMENT SIZE: " << sizeof(Type) << "\n";
    std::cout << "ELEMENT COUNT: " << COUNT << "\n";
    std::cout << std::endl;

    std::unordered_set<Type> stdset;
    LWE::stl::Set<Type>      lweset(LOAD_FACTOR);

    Bench bench, std_in, std_free, lwe_in, lwe_free;

    ///////////////////////////////////////////////////////////////////////////////
    // INSERT (INITIAL)
    ///////////////////////////////////////////////////////////////////////////////

    bench.loop([&]() {
        std::unordered_set<Type> temp;
        for(int i = 0; i < COUNT; ++i) temp.insert(i);
        volatile size_t size = temp.size(); // unoptimized
    });
    bench.output("STD INSERT (INITIAL) COUNT: ", COUNT);
    stlsec = bench.average();

    bench.loop([&]() {
        LWE::stl::Set<Type> temp(LOAD_FACTOR);
        for(int i = 0; i < COUNT; ++i) temp.push(i);
        volatile size_t size = temp.size(); // unoptimized
    });
    bench.output("LWE INSERT (INITIAL) COUNT: ", COUNT);
    bench.from(stlsec);

    ///////////////////////////////////////////////////////////////////////////////
    // WARM UP
    ///////////////////////////////////////////////////////////////////////////////

    for(int i = 0; i < COUNT; ++i) stdset.insert(i); // allocate
    for(int i = 0; i < COUNT; ++i) lweset.push(i);   // allocate
    stdset.clear();                                  // clear
    lweset.clear();                                  // clear

    // insert / clear loop 2 for average
    for(int i = 0; i < Bench<>::TRY; ++i) {
        // std insert
        std_in.once([&]() {
            for(int i = 0; i < COUNT; ++i) stdset.insert(i);
        });
        // std remove
        std_free.once([&]() {
            for(int i = 0; i < COUNT; ++i) stdset.erase(i);
        });
        // lwe insert
        lwe_in.once([&]() {
            for(int i = 0; i < COUNT; ++i) lweset.push(i);
        });
        // lwe remove
        lwe_free.once([&]() {
            for(int i = 0; i < COUNT; ++i) lweset.pop(i);
        });
    }

    ///////////////////////////////////////////////////////////////////////////////
    // INSERT ONCE
    ///////////////////////////////////////////////////////////////////////////////

    std_in.once([&]() {
        for(int i = 0; i < COUNT; ++i) stdset.insert(i);
    });
    std_in.output("STD INSERT (RESERVED) COUNT: ", COUNT);
    stlsec = std_in.average();

    lwe_in.once([&]() {
        for(int i = 0; i < COUNT; ++i) lweset.push(i);
    });
    lwe_in.output("LWE INSERT (RESERVED) COUNT: ", COUNT);
    lwe_in.from(stlsec);

    ///////////////////////////////////////////////////////////////////////////////
    // find
    ///////////////////////////////////////////////////////////////////////////////

    var = 0;
    bench.loop([&]() {
        for(int i = 0; i < FIND; ++i) {
            int value  = lwe::util::Random::generate(0, COUNT - 1); // ignore caching
            var       += stdset.find(value)->n;
        }
    });
    bench.output("STD FIND LOOP: ", FIND);
    stlsec = bench.average();

    var = 0;
    bench.loop([&]() {
        for(int i = 0; i < FIND; ++i) {
            int value  = lwe::util::Random::generate(0, COUNT - 1); // ignore caching
            var       += lweset.find(value)->n;
        }
    });
    bench.output("LWE FIND LOOP: ", FIND);
    bench.from(stlsec);

    ///////////////////////////////////////////////////////////////////////////////
    // ITERATE
    ///////////////////////////////////////////////////////////////////////////////

    var = 0;
    bench.loop([&]() {
        const auto end = stdset.end();
        for(auto itr = stdset.begin(); itr != end; ++itr) {
            var += itr->n;
        }
    });
    bench.output("STD READ BEGIN ~ END");
    stlsec = bench.average();

    var = 0;
    bench.loop([&]() {
        const auto end = lweset.end();
        for(auto itr = lweset.begin(); itr != end; ++itr) {
            var += itr->n;
        }
    });
    bench.output("LWE READ BEGIN ~ END");
    bench.from(stlsec);

    ///////////////////////////////////////////////////////////////////////////
    // clear once
    ///////////////////////////////////////////////////////////////////////////

    std_free.once([&]() {
        for(int i = 0; i < COUNT; ++i) stdset.erase(i);
    });
    std_free.output("STD CLEAR");
    stlsec = std_free.average();

    lwe_free.once([&]() {
        for(int i = 0; i < COUNT; ++i) lweset.pop(i);
    });
    lwe_free.output("LWE CLEAR");
    lwe_free.from(stlsec);

    std::cout << std::endl;
}

/**************************************************************************************************
 * hash collision test code
 **************************************************************************************************/
template<size_t SIZE, int COUNT> void test_collision() {
    float        sec = 0;
    volatile int var = 0;

    using Type = InvalidData<SIZE>;

    std::cout << "ELEMENT SIZE: " << sizeof(Type) << "\n";
    std::cout << "SAME DATA INSERT LOOP: " << COUNT << "\n";
    std::cout << std::endl;


    std::unordered_set<Type> stdset;
    LWE::stl::Set<Type>      lweset(LOAD_FACTOR);

    Bench bench;

    float stlsec;

    stdset.insert(Type{ 100 }); // insert dummy
    lweset.insert(Type{ 100 }); // insert dummy

    bench.loop([&]() {
        for(int i = 0; i < COUNT; ++i) {
            stdset.insert(100); // same value
        }
    });
    bench.output("STD INSERT SAME VALUE LOOP: ", COUNT);
    stlsec = bench.average();

    bench.loop([&]() {
        for(int i = 0; i < COUNT; ++i) {
            lweset.push(100); // same value
        }
    });
    bench.output("LWE INSERT SAME VALUE LOOP: ", COUNT);
    bench.from(stlsec);

    std::cout << "std count check: " << stdset.size() << "\n";
    std::cout << "lwe count check: " << lweset.size() << "\n";
    
    std::cout << std::endl;
}
