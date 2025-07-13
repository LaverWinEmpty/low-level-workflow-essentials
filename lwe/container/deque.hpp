/**************************************************************************************************
 * circulation container
 *
 * description
 * use small vector
 * random accessible
 * has iterator and reverse iterator
 *
 * mix of signed and unsigned.
 * it is safe from overflow by using bitwise operations.
 * e.g
 *  index: 0 -> -1
 *  0b0 -> 0b1111...1111
 *  0b1111...1111 & 0b1111 (capacity is 16)
 *  index: -1 -> 15
 **************************************************************************************************
 * example
 *
 * capacitor -> power of 2
 * [0] [1] [2] [3] [4] [5] [6] [7]
 *  a   b   X   Y   Z   c   d   e
 *          ^       ^   ^
 *          begin   |   end
 *          front   rear
 *          top     bottom
 *
 * iterator
 *   X -> Y -> Z -> X ... (cycle only within range)
 *
 * rear and bottom
 * - last index getter
 * - return reverse iterator
 * - if to point to end(), sub 1 (auto itr = rear() - 1;)
 * - if going to begin(), add (auto itr = rear(); ++itr;)
 *
 * indexing
 *  [0] -> X (2)
 *  [2] -> Z (4)
 *  [3] -> c (5, out of range)
 *  [8] -> X (2, circulation)
 **************************************************************************************************
 * feature
 *
 * - push_back:  push
 * - pop_back:   pop
 * - push_front: unshift
 * - pop_front:  shift
 *
 * exception
 * - push return bool -> allocation failed
 * - pop return bool -> not exist data
 *
 * performance
 * - push/pop: O(1) amortized, O(n) worst case (reallocation)
 * - indexing overhead: relative address convert to absolute (bitmask optimized)
 * - call realloc() instead of new() (possible optimization)
 **************************************************************************************************/

#ifndef LWE_CONTAINER_VECTOR
#define LWE_CONTAINER_VECTOR

#include "../config/config.h"
#include "../mem/block.hpp"
#include "iterator.hpp"

LWE_BEGIN
namespace container {

template<typename T, size_t SVO = 0> class Deque {
    template<typename, size_t> friend class Deque; //!< for Deuqe<T, OTHER_SVO_SIZE>

public:
    CONTAINER_BODY(Deque, T, T, SVO);

protected:
    static constexpr size_t MIN = SVO == 0 ? 0 : (SVO < config::SMALLVECTOR ? config::SMALLVECTOR : align(SVO));

public:
    Deque();
    ~Deque();

public:
    template<size_t N> Deque(const Deque<T, N>&);
    template<size_t N> Deque(Deque<T, N>&&);
    template<size_t N> Deque<T, N>& operator=(const Deque<T, N>&);
    template<size_t N> Deque<T, N>& operator=(Deque<T, N>&&);

public:
    T& operator[](index_t) const noexcept; //!< circulation operator[], error: count == 0

public:
    template<typename Arg> bool emplace(index_t, Arg&&) noexcept; //!< out of range: push_back or push_front
    bool                        erase(index_t, T* = nullptr) noexcept;

public:
    bool push() noexcept;  //!< push_back default constructor
    bool shift() noexcept; //!< push_front default constructor

public:
    bool push(T&&) noexcept;       //!< push_back
    bool push(const T&) noexcept;  //!< push_back
    bool shift(T&&) noexcept;      //!< push_front
    bool shift(const T&) noexcept; //!< push_front

public:
    bool pop(T* = nullptr) noexcept;  //!< pop_back
    bool pop(T&) noexcept;            //!< pop_back
    bool pull(T* = nullptr) noexcept; //!< pop_front
    bool pull(T&) noexcept;           //!< pop_front

public:
    bool resize(size_t) noexcept;  //!< realloc
    bool reserve(size_t) noexcept; //!< realloc
    bool compact() noexcept;       //!< realloc
    void clear() noexcept;         //!< not free and realloc

public:
    size_t size() const noexcept;     //!< emement count == size
    size_t capacity() const noexcept; //!< container size
    bool   full() const noexcept;     //!< check full
    bool   empty() const noexcept;    //!< check empty

public:
    T*       data() noexcept;       //!< container address
    T&       at(index_t);           //!< at
    const T* data() const noexcept; //!< container address const
    const T& at(index_t) const;     //!< at const

public:
    Iterator<FWD | VIEW> begin() const noexcept;  //!< iterator begin
    Iterator<FWD | VIEW> end() const noexcept;    //!< iterator end
    Iterator<BWD | VIEW> rbegin() const noexcept; //!< reverse iterator begin
    Iterator<BWD | VIEW> rend() const noexcept;   //!< reverse iterator end

public:
    Iterator<FWD | VIEW> front() const noexcept;  //!< iterator for head to tail
    Iterator<BWD | VIEW> rear() const noexcept;   //!< iterator for tail to head
    Iterator<BWD | VIEW> top() const noexcept;    //!< iterator for tail to head
    Iterator<FWD | VIEW> bottom() const noexcept; //!< iterator for head to tail

public:
    Iterator<FWD> begin() noexcept;  //!< iterator begin
    Iterator<FWD> end() noexcept;    //!< iterator end
    Iterator<BWD> rbegin() noexcept; //!< reverse iterator begin
    Iterator<BWD> rend() noexcept;   //!< reverse iterator end

public:
    Iterator<FWD> front() noexcept;  //!< iterator for head to tail
    Iterator<BWD> rear() noexcept;   //!< iterator for tail to head
    Iterator<BWD> top() noexcept;    //!< iterator for tail to head
    Iterator<FWD> bottom() noexcept; //!< iterator for head to tail

public:
    template<typename U> void push_back(U&&);  //!< STL compatible
    template<typename U> void push_front(U&&); //!< STL compatible
    void                      pop_back();      //!< STL compatible
    void                      pop_front();     //!< STL compatible

protected:
    index_t forward(index_t) noexcept;        //!< out: in - 1
    index_t backward(index_t) noexcept;       //!< out: in + 1
    index_t relative(index_t) const noexcept; //!< out: index 0 to head, circulation 0 ~ capacity
    index_t clamp(index_t) const noexcept;    //!< out: index 0 to head, clamped to head ~ tail

protected:
    bool reallocate(size_t) noexcept; //!< call realloc function

protected:
    union {
        mem::Block<MIN, T> stack; // stack, union for uninitialize
    };
    T*      container = nullptr; //!< container
    size_t  capacitor = 0;       //!< size: container
    index_t counter   = 0;       //!< size: element
    index_t head      = 0;       //!< index: front / bottom
    index_t tail      = -1;      //!< index: rear / top
};

} // namespace container
LWE_END
#include "deque.ipp"
#endif
