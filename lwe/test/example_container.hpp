#pragma once

#include "../container/ring_buffer.hpp"   // default container
#include "../container/linear_buffer.hpp" // default container
#include "../container/hashed_buffer.hpp" // default container
#include "../container/hash_table.hpp"    // default container

#include "example_reflection.hpp" // Test class reuse
#include "../stl/stack.hpp"       // included meta.h
#include "../stl/deque.hpp"       // included meta.h
#include "../stl/set.hpp"         // included meta.h
#include "../stl/map.hpp"         // included meta.h

// REGISTER OTHER CONTAINER
template<typename T> struct StdVectorAdapter : std::vector<T> {
    // NEED `push` METHOD
    template<typename U> void push(U&&in) { this->emplace_back(std::forward<U>(in)); }
};
// DECLARE_CONTAINER  (TEMPLATE), NAME, BASE, TPARAM...
// REGISTER_CONTAINER (TEMPLATE), NAME, KEYWORD, TPARAM...
DECLARE_CONTAINER((typename T = void), MyVector, StdVectorAdapter, T);
REGISTER_CONTAINER((typename T), MyVector, Keyword::STL_VECTOR, T);

namespace test {
    using namespace lwe::stl;
    using namespace lwe::container;

void example_container() { 
    /* === QUICK START === */
    MyVector<int> v;
    v.deserialize("[ 1, 2, 3 ]"); // std::vector serializable
    std::cout << v.serialize();   // test [ 1, 2, 3 ]

    ///* === DETAIL === */
    Stack<int>    lwevec; // vector, deque (ring buffer)
    Deque<int>    lwedeq; // vector, deque (ring buffer)
    Set<int>      lweset; // unordered_set
    Map<int, int> lwemap; // unordered_map

    // DEFAULT CONTAINER (NOT SERIALIZABLE)

    container::LinearBuffer<int>   linearBuffer; // stack (vector)
    container::RingBuffer<int>     ringBuffer;   // deque
    container::HashedBuffer<int>   hashBuffer;   // hash set
    container::HashTable<int, int> hashTable;    // hash set

    linearBuffer.push(); // push_back (empty)
    linearBuffer.pop();  // pop_back  (no get)

    ringBuffer.push();    // push_back (empty)
    ringBuffer.pop();     // pop_back  (no get)
    ringBuffer.pull();    // pop_front (no get)
    ringBuffer.prepend(); // push_front

    hashBuffer.push(0); // insert
    hashBuffer.pop(0);  // erase

    hashTable.push(0, 1);     // insert
    hashTable.push({ 0, 1 }); // insert
    hashTable.pop(0);         // erase
}

} //