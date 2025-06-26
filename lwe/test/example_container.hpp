#pragma once

#include "example_reflection.hpp" // Test class reuse
#include "../stl/deque.hpp"       // include meta.h

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
    lwe::stl::Deque<Test, 8> testArray; // Small Vector Optimization: size 8
    // Storage larger than SVO size -> Heap allocation

    testArray.push({ 1, 1, 1 });
    testArray.push({ 2, 2, 2 });
    testArray.push({ 3, 3, 3 });
    testArray.push({ 4, 4, 4 });

    // or pop_front for standard
    testArray.shift();
    // Supported: both pointer and reference
    // out == nullptr -> abandon (defualt)
    // Data is not moved, but the starting index is set to 1.

    // count 9, size 3 -> circulation
    for(int i = 0; i < 9; ++i) {
        std::cout << testArray[i].c << " ";
    }
    std::cout << "\n"; // 2 3 4 0 0 0 0 0 1 2
    // CAREFUL: Potential UB
    //  - [0] is not removed from memory but destructor was called
    //    (Accessing destructed objects may cause errors)
    // - Out of range behavior: no throw, uses circulation
    //   index within capacity -> allowed
    //   index beyond capacity -> wraps around (modulo)
    // - Design rationale:
    //   1. Prevent crashes while allowing controlled out-of-bounds access
    //   2. Support index-based access after reserve()/resize() operations
    //   3. Enable fixed-size circular buffer usage (capacity = power of 2)
    //      (Note: This use case is not recommended for general purposes)

    // if need throw, use at()
    try {
        std::cout << testArray.at(1'000'000).c;
    }
    catch(std::out_of_range& e) {
        // catched
        std::cout << e.what() << "\n";
    }

    // cout 6 -> circulation
    auto itr = testArray.begin();
    for(int i = 0; i < 6; ++i) {
        std::cout << itr->c << " ";
        ++itr;
    }
    std::cout << "\n"; // 2 3 4 2 3 4 (Valid)

    // style -> stack
    auto stackItr = testArray.top();
    for(auto itr = testArray.front(); itr != testArray.rear() - 1; ++itr) {
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
    // - e.g. size() == 10, end index == 10
    // rear, bottom is in of range
    // - e.g. size() == 10, end index == 9
    // It is like rbegin, rend, need itr - 1
    // Or, read itr finally from outside the loop.
    // Because, rear and bottom are iterators, but also mean "last element."
    //
    // Note 2
    // Iterator += value is supported.

    // NOTE
    auto beginTest = testArray.begin();                             // get testArray[0];
    std::cout << "array old pointer: " << testArray.data() << "\n"; // before pointer
    testArray.reserve(100);                                         // 100 -> set 128, realloc T[128]
    std::cout << "array new pointer: " << testArray.data() << "\n"; // after pointer
    std::cout << beginTest->c << std::endl;                         // not dangling pointer

    // serialize
    std::cout << testArray.serialize();

    // out (console)
    /*
        2 3 4 0 0 0 0 0 1 2
        out of range
        2 3 4 2 3 4
        2 3 4
        array old pointer: 0000000000000000
        array new pointer: 1111111111111111
        2
        [{ 2, 2 }, { 3, 3 }, { 4, 4 }]
    */
}

} // namespace test
