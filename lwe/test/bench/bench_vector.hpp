#include "iostream"

#include "vector"
#include "C:\.workspace\git\low-level-workflow-essesntials\lwe\container\stack.hpp"
#include "C:\.workspace\git\low-level-workflow-essesntials\lwe\test\bench\internal\bench.hpp"

#define RING_BUFFER

using namespace lwe::container;

template<size_t N> struct Buffer {
    static constexpr size_t SIZE = N;

    Buffer(int n = 0): n(n) { }

    int  n;
    char buffer[N - sizeof(n)] = { 0 };
};

static constexpr size_t SIZE  = 8;
static constexpr size_t COUNT = lwe::core::align(0x7f'ff'ff'ff / SIZE) >> 1;

using Std    = std::vector<Buffer<SIZE>>;
using Lwe    = lwe::container::Stack<Buffer<SIZE>>;
using LweSVO = lwe::container::Stack<Buffer<SIZE>, COUNT>;

// SVO: slower iteration for 128-byte elements, equivalent for 8-byte, cause unknown
LweSVO lwesvo;

int main() {
    Bench b, std_push, std_pop, lwe_push, lwe_pop, svo_push, svo_pop;

    std::cout << "ELEMENT SIZE:  " << SIZE << "\n"
              << "ELEMENT COUNT: " << COUNT << "\n";

    volatile size_t dummy;

    /***********************************************************************************************
     * PUSH TEST (NO CACHED)
     ***********************************************************************************************/

    for(int i = 0; i < Bench<>::TRY; ++i) {
        Std stdvec;
        Lwe lwevec;

        std_push.once([&]() {
            for(int i = 0; i < COUNT; ++i) stdvec.push_back(i); // push
            dummy = stdvec.size();                              // read
        });
        lwe_push.once([&]() {
            for(int i = 0; i < COUNT; ++i) lwevec.push_back(i); // push
            dummy = lwevec.size();                              // read
        });
    }

    std_push.output("STD_PUSH (NO CACHE)");
    lwe_push.output("LWE_PUSH (NO CACHE)");

    std::cout << std::endl;

    /***********************************************************************************************
     * PUSH POP PRE
     ***********************************************************************************************/

    // reserve
    Std stdvec;
    Lwe lwevec;
    stdvec.reserve(COUNT);
    lwevec.reserve(COUNT);

    // warm-up
    for(int i = 0; i < COUNT; ++i) stdvec.push_back(i);
    for(int i = 0; i < COUNT; ++i) stdvec.pop_back();
    for(int i = 0; i < COUNT; ++i) lwevec.push_back(i);
    for(int i = 0; i < COUNT; ++i) lwevec.pop_back();
    for(int i = 0; i < COUNT; ++i) lwesvo.push_back(i);
    for(int i = 0; i < COUNT; ++i) lwesvo.pop_back();

    for(int i = 0; i < b.TRY - 1; ++i) {
        // STD
        std_push.once([&]() {
            for(int i = 0; i < COUNT; ++i) stdvec.push_back(i); // push
        });
        dummy = stdvec.size(); // read
        std_pop.once([&]() {
            for(int i = 0; i < COUNT; ++i) stdvec.pop_back(); // pop
        });
        dummy = stdvec.size(); // read

        // LWE
        lwe_push.once([&]() {
            for(int i = 0; i < COUNT; ++i) lwevec.push_back(i); // push
        });
        dummy = lwevec.size(); // read
        lwe_pop.once([&]() {
            for(int i = 0; i < COUNT; ++i) lwevec.pop_back(); // pop
        });
        dummy = lwevec.size(); // read

        // LWE SVO
        svo_push.once([&]() {
            for(int i = 0; i < COUNT; ++i) lwesvo.push_back(i); // push
        });
        dummy = lwesvo.size(); // read
        svo_pop.once([&]() {
            for(int i = 0; i < COUNT; ++i) lwesvo.pop_back(); // pop
        });
        dummy = lwesvo.size(); // read
    }

    /***********************************************************************************************
     * PUSH LAST
     ***********************************************************************************************/
    // STD
    std_push.once([&]() {
        for(int i = 0; i < COUNT; ++i) stdvec.push_back(i); // push
    });
    dummy = stdvec.size(); // read
    // LWE
    lwe_push.once([&]() {
        for(int i = 0; i < COUNT; ++i) lwevec.push_back(i); // push
    });
    dummy = lwevec.size(); // read
    // LWE SVO
    svo_push.once([&]() {
        for(int i = 0; i < COUNT; ++i) lwesvo.push_back(i); // push
    });
    dummy = lwesvo.size(); // read

    /***********************************************************************************************
     * INDEXING TEST
     ***********************************************************************************************/
    dummy = 0;
    b.loop([&]() {
        for(int i = 0; i < COUNT; ++i) dummy += stdvec[i].n; // std vec read
    });
    b.output("STD INDEXING");

    dummy = 0;
    b.loop([&]() {
        for(int i = 0; i < COUNT; ++i) dummy += lwevec[i].n; // std vec read
    });
    b.output("LWE INDEXING");

    dummy = 0;
    b.loop([&]() {
        for(int i = 0; i < COUNT; ++i) dummy += lwesvo[i].n; // std vec read
    });
    b.output("SVO INDEXING");

    std::cout << std::endl;

    /***********************************************************************************************
     * ITERATE TEST
     ***********************************************************************************************/
    dummy = 0;
    b.loop([&]() {
        for(auto& itr : stdvec) dummy += itr.n; // std vec read
    });
    b.output("STD ITERATE");

    dummy = 0;
    b.loop([&]() {
        for(auto& itr : lwevec) dummy += itr.n; // std vec read
    });
    b.output("LWE ITERATE");

    dummy = 0;
    b.loop([&]() {
        for(auto& itr : lwesvo) dummy += itr.n; // std vec read
    });
    b.output("SVO ITERATE");

    std::cout << std::endl;

    /***********************************************************************************************
     * POP LAST
     ***********************************************************************************************/
    // STD
    std_pop.once([&]() {
        for(int i = 0; i < COUNT; ++i) stdvec.pop_back(); // pop
    });
    dummy = stdvec.size(); // read
    // LWE
    lwe_pop.once([&]() {
        for(int i = 0; i < COUNT; ++i) lwevec.pop_back(); // pop
    });
    dummy = lwevec.size(); // read
    // LWE SVO
    svo_pop.once([&]() {
        for(int i = 0; i < COUNT; ++i) lwesvo.pop_back(); // pop
    });
    dummy = lwesvo.size(); // read

    std_push.output("STD_PUSH (RESERVED + CACHE)");
    lwe_push.output("LWE_PUSH (RESERVED + CACHE)");
    svo_push.output("SVO_PUSH (RESERVED + CACHE)");
    std_pop.output("STD_POP");
    lwe_pop.output("LWE_POP");
    svo_pop.output("SVO_POP");

    std::cout << std::endl;
}
