#pragma once

#include "example_reflection.hpp" // Test class reuse
#include "../stl/deque.hpp" // include meta.h

// main
namespace test {

void example_container() {
    using namespace lwe;

    // Currently we only have deque.

    // deque test
    // My Deque
    // Circular Array
    // declare: Deque<Type, SVO size>
    // capacity is power of 2, optimized
    lwe::stl::Deque<Test, 8> testArray; // SVO 8, over -> allocate

    testArray.push({ 1, 1, 1 });
    testArray.push({ 2, 2, 2 });
    testArray.push({ 3, 3, 3 });
    testArray.push({ 4, 4, 4 });
    
    // or pop_front for standard
    testArray.shift();
    // Supported: both pointer and reference
    // out == nullptr -> abandon (defualt)
    // Data is not moved, but the starting index is set to 1.

    // count 8 -> out of range (UB)
    for (int i = 0; i < 8; ++i) {
        std::cout << testArray[i].c << " ";
    }
    std::cout << "\n"; // 2 3 4 0 0 0 0 0 1 (UB)
    // [0] is not removed
    // out of range -> no throw, circulation

    // cout 6 -> circulation
    auto itr = testArray.begin();
    for (int i = 0; i < 6; ++i) {
        std::cout << itr->c << " ";
        ++itr;
    }
    std::cout << "\n"; // 2 3 4 2 3 4 (Valid)

    // style -> stack
    auto stackItr= testArray.top();
    for (auto itr = testArray.front(); itr != testArray.rear() - 1; ++itr) {
        std::cout << itr->c << " ";
    }
    std::cout << "\n"; // 4 3 2 (Valid)
    // surpported
    // begin  -> end
    // rbegin -> rend
    // top    -> bottom (stack style)
    // front  -> rear   (queue style)
    //
    // Note 1
    // end, rend is out of range
    // - e.g. size() == 10, end index == 11
    // rear, bottom is in of range
    // - e.g. size() == 10, end index == 10
    // It is like rbegin, rend, need itr - 1
    // Or, read itr finally from outside.
    //
    // Note 2
    // Iterator += value is supported.

    // serialize
    std::cout << testArray.serialize();

    // out (console)
    /*
        2 3 4 0 0 0 0 0 1
        2 3 4 2 3 4
        2 3 4
        [{ 2, 2 }, { 3, 3 }, { 4, 4 }]
    */
}

}