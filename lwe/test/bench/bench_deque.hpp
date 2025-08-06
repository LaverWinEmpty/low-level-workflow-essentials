#pragma once

#include "internal/bench.hpp"

#include "deque"
#include "../../container/ring_buffer.hpp"

using namespace lwe::container;

template<size_t N> struct Buffer {
    static constexpr size_t SIZE = N;

    Buffer(int n = 0): n(n) { }

    int  n;
    char buffer[N - sizeof(n)] = { 0 };
};

static constexpr size_t SIZE  = 128;
static constexpr size_t COUNT = 5'000'000;

using Std    = std::deque<Buffer<SIZE>>;
using Lwe    = lwe::container::RingBuffer<Buffer<SIZE>>;
using LweSVO = lwe::container::RingBuffer<Buffer<SIZE>, COUNT>;

LweSVO lwesvo;

int main() {
    Bench::introduce();

    Bench b, std_push, std_pop, lwe_push, lwe_pop, svo_push, svo_pop;

    std::cout << "ELEMENT SIZE:  " << SIZE << "\n"
              << "ELEMENT COUNT: " << COUNT << "\n";

    volatile size_t dummy;

    /***********************************************************************************************
     * PUSH TEST (NO CACHED)
     ***********************************************************************************************/

    for(int i = 0; i < Bench::TRY; ++i) {
        Std stdvec;
        Lwe lwevec;

        std_push.once([&]() {
            for(int i = 0; i < COUNT; ++i) stdvec.push_front(i); // push
            dummy = stdvec.size();                               // read
        });
        lwe_push.once([&]() {
            for(int i = 0; i < COUNT; ++i) {
                lwevec.push_front(i); // push
            }
            dummy = lwevec.size(); // read
        });
    }

    std_push.output("STD PUSH FRONT (NO CACHE)");
    lwe_push.output("LWE PUSH FRONT (NO CACHE)");

    std::cout << std::endl;

    /***********************************************************************************************
     * PUSH POP PRE
     ***********************************************************************************************/

    // reserve
    Std stdvec;
    Lwe lwevec;
    // stdvec.reserve(COUNT); // impossible
    lwevec.reserve(COUNT);

    // warm-up
    for(int i = 0; i < COUNT; ++i) stdvec.push_front(i);
    for(int i = 0; i < COUNT; ++i) stdvec.pop_front();
    for(int i = 0; i < COUNT; ++i) lwevec.push_front(i);
    for(int i = 0; i < COUNT; ++i) lwevec.pop_front();
    for(int i = 0; i < COUNT; ++i) lwesvo.push_front(i);
    for(int i = 0; i < COUNT; ++i) lwesvo.pop_front();

    for(int i = 0; i < b.TRY - 1; ++i) {
        // STD
        std_push.once([&]() {
            for(int i = 0; i < COUNT; ++i) stdvec.push_front(i); // push
        });
        dummy = stdvec.size(); // read
        std_pop.once([&]() {
            for(int i = 0; i < COUNT; ++i) stdvec.pop_front(); // pop
        });
        dummy = stdvec.size(); // read

        // LWE
        lwe_push.once([&]() {
            for(int i = 0; i < COUNT; ++i) lwevec.push_front(i); // push
        });
        dummy = lwevec.size(); // read
        lwe_pop.once([&]() {
            for(int i = 0; i < COUNT; ++i) lwevec.pop_front(); // pop
        });
        dummy = lwevec.size(); // read

        // LWE SVO
        svo_push.once([&]() {
            for(int i = 0; i < COUNT; ++i) lwesvo.push_front(i); // push
        });
        dummy = lwesvo.size(); // read
        svo_pop.once([&]() {
            for(int i = 0; i < COUNT; ++i) lwesvo.pop_front(); // pop
        });
        dummy = lwesvo.size(); // read
    }

    /***********************************************************************************************
     * PUSH LAST
     ***********************************************************************************************/
    // STD
    std_push.once([&]() {
        for(int i = 0; i < COUNT; ++i) stdvec.push_front(i); // push
    });
    dummy = stdvec.size(); // read
    // LWE
    lwe_push.once([&]() {
        for(int i = 0; i < COUNT; ++i) lwevec.push_front(i); // push
    });
    dummy = lwevec.size(); // read
    // LWE SVO
    svo_push.once([&]() {
        for(int i = 0; i < COUNT; ++i) lwesvo.push_front(i); // push
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
        for(int i = 0; i < COUNT; ++i) stdvec.pop_front(); // pop
    });
    dummy = stdvec.size(); // read
    // LWE
    lwe_pop.once([&]() {
        for(int i = 0; i < COUNT; ++i) lwevec.pop_front(); // pop
    });
    dummy = lwevec.size(); // read
    // LWE SVO
    svo_pop.once([&]() {
        for(int i = 0; i < COUNT; ++i) lwesvo.pop_front(); // pop
    });
    dummy = lwesvo.size(); // read

    std_push.output("STD PUSH FRONT (RESERVED + CACHE)");
    lwe_push.output("LWE PUSH FRONT (RESERVED + CACHE)");
    svo_push.output("SVO PUSH FRONT (RESERVED + CACHE)");
    std_pop.output("STD POP FRONT");
    lwe_pop.output("LWE POP FRONT");
    svo_pop.output("SVO POP FRONT");

    std::cout << std::endl;
}
