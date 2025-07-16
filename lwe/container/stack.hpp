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

//! @tparam N count of T, 0 is auto size (64 byte)
template<typename T, size_t N = 0>
class Stack {
    template<typename, size_t> friend class Stack; //!< for Deuqe<T, OTHER_SVO_SIZE>

public:
    CONTAINER_BODY(Stack, T, T, N);

private:
    static constexpr size_t min() {
        constexpr size_t ALIGNED = align(config::SMALLVECTOR / sizeof(T));
        return ALIGNED < 4 ? 0 : ALIGNED;
    }

public:
    static constexpr size_t SVO = N;
    static constexpr size_t MIN = min();

public:
    Stack();
    Stack(const Stack&);
    Stack(Stack&&) noexcept;
    Stack& operator=(const Stack&);
    Stack& operator=(Stack&&) noexcept;
    ~Stack();

public:
    template<size_t N> Stack(const Stack<T, N>&);
    template<size_t N> Stack(Stack<T, N>&&) noexcept;
    template<size_t N> Stack<T, N>& operator=(const Stack<T, N>&);
    template<size_t N> Stack<T, N>& operator=(Stack<T, N>&&) noexcept;

public:
    T& operator[](index_t) noexcept;             //!<
    const T& operator[](index_t) const noexcept; //!<

public:
    template<typename Arg> bool emplace(index_t, Arg&&) noexcept; //!< out of range: push_back or push_front
    bool                        erase(index_t, T* = nullptr) noexcept;

public:
    bool push() noexcept;  //!< push_back default constructor
    bool shift() noexcept; //!< push_front default constructor

public:
    bool push(T&&) noexcept;      //!< push_back
    bool push(const T&) noexcept; //!< push_back

public:
    bool pop(T* = nullptr) noexcept; //!< pop_back
    bool pop(T&) noexcept;           //!< pop_back

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
    Iterator<FWD> begin() noexcept;  //!< iterator begin
    Iterator<FWD> end() noexcept;    //!< iterator end
    Iterator<BWD> rbegin() noexcept; //!< reverse iterator begin
    Iterator<BWD> rend() noexcept;   //!< reverse iterator end

public:
    const T* front() const noexcept;  //!< iterator for head to tail
    const T* rear() const noexcept;   //!< iterator for tail to head
    const T* top() const noexcept;    //!< iterator for tail to head
    const T* bottom() const noexcept; //!< iterator for head to tail

public:
    T* front() noexcept;  //!< iterator for head to tail
    T* rear() noexcept;   //!< iterator for tail to head
    T* top() noexcept;    //!< iterator for tail to head
    T* bottom() noexcept; //!< iterator for head to tail

public:
    template<typename U> void push_back(U&&); //!< STL compatible
    void                      pop_back();     //!< STL compatible

private:
    template<size_t X, bool COPY> using Other = std::conditional_t<COPY, const Stack<T, X>&, Stack<T, X>&&>;
    template<size_t X, bool COPY> bool ctor(Other<X, COPY>, size_t = 0);           //!< copy / move delegator
    template<bool> void                transfer(const T*, T*, size_t, size_t = 0); //!< @tparam true: copy, false: move
    bool                               reallocate(size_t, size_t = 0);             //!< call realloc (size, begin)

protected:
    void clear(size_t);

protected:
    union {
        mem::Block<MIN, T> stack; // memory stack, union for uninitialize
    };
    T*      container = nullptr; //!< container
    size_t  capacitor = MIN;     //!< size: container
    index_t counter   = 0;       //!< size: element
};

} // namespace container
LWE_END
#include "vector.ipp"
#endif
