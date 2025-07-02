#ifndef LWE_CONTAINER_DEQUE
#define LWE_CONTAINER_DEQUE

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

#include "config.hpp"
#include "../base/base.h"
#include "../meta/meta.h"
#include "../mem/Block.hpp"

LWE_BEGIN
namespace stl {

template<typename T, size_t SVO = 0> class Deque: public meta::Container {
    CONTAINER_BODY(T, Deque, T, SVO); // <T, SVO>

private:
    template<typename, size_t> friend class Deque;

private:
    static constexpr size_t MIN = SVO == 0 ? 0 : (SVO < DEF_SVO ? DEF_SVO : align(SVO));

public:
    class FwdIterW; //!< forward iterator writable == iterator
    class FwdIterR; //!< forward iterator readonly == const iterator
    class BwdIterW; //!< backward iterator writable == reverse iterator
    class BwdIterR; //!< backward iterator readonly == const reverse iterator
    friend class FwdIterW;
    friend class BwdIterW;

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
    bool push() noexcept;    //!< push_back default constructor
    bool unshift() noexcept; //!< push_front default constructor

public:
    bool push(T&&) noexcept;         //!< push_back
    bool push(const T&) noexcept;    //!< push_back
    bool unshift(T&&) noexcept;      //!< push_front
    bool unshift(const T&) noexcept; //!< push_front

public:
    bool pop(T* = nullptr) noexcept;   //!< pop_back
    bool pop(T&) noexcept;             //!< pop_back
    bool shift(T* = nullptr) noexcept; //!< pop_front
    bool shift(T&) noexcept;           //!< pop_front

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
    T* data() const noexcept; //!< container address
    T& at(index_t) const;

public:
    FwdIterR begin() const noexcept;  //!< iterator begin
    FwdIterR end() const noexcept;    //!< iterator end
    BwdIterR rbegin() const noexcept; //!< reverse iterator begin
    BwdIterR rend() const noexcept;   //!< reverse iterator end

public:
    FwdIterR front() const noexcept;  //!< iterator for head to tail
    BwdIterR rear() const noexcept;   //!< iterator for tail to head
    BwdIterR top() const noexcept;    //!< iterator for tail to head
    FwdIterR bottom() const noexcept; //!< iterator for head to tail

public:
    FwdIterW begin() noexcept;  //!< iterator begin
    FwdIterW end() noexcept;    //!< iterator end
    BwdIterW rbegin() noexcept; //!< reverse iterator begin
    BwdIterW rend() noexcept;   //!< reverse iterator end

public:
    FwdIterW front() noexcept;  //!< iterator for head to tail
    BwdIterW rear() noexcept;   //!< iterator for tail to head
    BwdIterW top() noexcept;    //!< iterator for tail to head
    FwdIterW bottom() noexcept; //!< iterator for head to tail

public:
    template<typename U> void push_back(U&&);  //!< STL compatible
    template<typename U> void push_front(U&&); //!< STL compatible
    void                      pop_back();      //!< STL compatible
    void                      pop_front();     //!< STL compatible

private:
    index_t forward(index_t) noexcept;        //!< out: in - 1
    index_t backward(index_t) noexcept;       //!< out: in + 1
    index_t relative(index_t) const noexcept; //!< out: index 0 to head, circulation 0 ~ capacity
    index_t clamp(index_t) const noexcept;    //!< out: index 0 to head, clamped to head ~ tail

private:
    bool reallocate(size_t) noexcept; //!< call realloc function

private:
    union {
        mem::Block<MIN, T> stack; // stack, union for uninitialize
    };
    T*      container = nullptr; //!< container
    size_t  capacitor = 0;       //!< size: container
    index_t counter   = 0;       //!< size: element
    index_t head      = 0;       //!< index: front / bottom
    index_t tail      = -1;      //!< index: rear / top
};

template<typename T, size_t SVO> class Deque<T, SVO>::FwdIterW {
    friend class BwdIterW;
public:
    FwdIterW(const Deque*, index_t) noexcept;
    FwdIterW(const FwdIterW&) noexcept;
    FwdIterW(const BwdIterW&) noexcept;
    FwdIterW& operator=(const FwdIterW&) noexcept;
    FwdIterW& operator=(const BwdIterW&) noexcept;
    FwdIterW& operator++() noexcept;
    FwdIterW& operator--() noexcept;
    FwdIterW  operator++(int) noexcept;
    FwdIterW  operator--(int) noexcept;
    FwdIterW  operator+(index_t) const noexcept;
    FwdIterW  operator-(index_t) const noexcept;
    FwdIterW& operator+=(index_t) noexcept;
    FwdIterW& operator-=(index_t) noexcept;
    bool     operator==(const FwdIterW&) const noexcept;
    bool     operator!=(const FwdIterW&) const noexcept;
    bool     operator==(const BwdIterW&) const noexcept;
    bool     operator!=(const BwdIterW&) const noexcept;
    bool     operator<(const FwdIterW&) const noexcept;
    bool     operator>(const FwdIterW&) const noexcept;
    bool     operator<=(const FwdIterW&) const noexcept;
    bool     operator>=(const FwdIterW&) const noexcept;
    bool     operator<(const BwdIterW&) const noexcept;
    bool     operator>(const BwdIterW&) const noexcept;
    bool     operator<=(const BwdIterW&) const noexcept;
    bool     operator>=(const BwdIterW&) const noexcept;
    const T& operator*() const noexcept;
    const T* operator->() const noexcept;
    T&       operator*() noexcept;
    T*       operator->() noexcept;
private:
    const Deque* outer;
    index_t      index;
};

template<typename T, size_t SVO> class Deque<T, SVO>::BwdIterW {
    friend class FwdIterW;
public:
    BwdIterW(const Deque*, index_t) noexcept;
    BwdIterW(const BwdIterW&) noexcept;
    BwdIterW(const FwdIterW&) noexcept;
    BwdIterW& operator=(const BwdIterW&) noexcept;
    BwdIterW& operator=(const FwdIterW&) noexcept;
    BwdIterW& operator++() noexcept;
    BwdIterW& operator--() noexcept;
    BwdIterW  operator++(int) noexcept;
    BwdIterW  operator--(int) noexcept;
    bool      operator==(const BwdIterW&) const noexcept;
    bool      operator!=(const BwdIterW&) const noexcept;
    bool      operator==(const FwdIterW&) const noexcept;
    bool      operator!=(const FwdIterW&) const noexcept;
    bool      operator<(const BwdIterW&) const noexcept;
    bool      operator>(const BwdIterW&) const noexcept;
    bool      operator<=(const BwdIterW&) const noexcept;
    bool      operator>=(const BwdIterW&) const noexcept;
    bool      operator<(const FwdIterW&) const noexcept;
    bool      operator>(const FwdIterW&) const noexcept;
    bool      operator<=(const FwdIterW&) const noexcept;
    bool      operator>=(const FwdIterW&) const noexcept;
    BwdIterW  operator+(index_t) const noexcept;
    BwdIterW  operator-(index_t) const noexcept;
    BwdIterW& operator+=(index_t) noexcept;
    BwdIterW& operator-=(index_t) noexcept;
    const T& operator*() const noexcept;
    const T* operator->() const noexcept;
    T&       operator*() noexcept;
    T*       operator->() noexcept;
private:
    FwdIterW iterator;
};

template<typename T, size_t SVO> class Deque<T, SVO>::FwdIterR: public FwdIterW {
    using FwdIterW::FwdIterW;

public:
    FwdIterR(const FwdIterW& in): // writable -> readonly
        FwdIterW(in) { }
    FwdIterR(const BwdIterR& in): // backward -> forward
        FwdIterW(in) { }
    const T& operator*() { return FwdIterW::operator*(); }   //!< non-const hide
    const T* operator->() { return FwdIterW::operator->(); } //!< non-const hide
};

template<typename T, size_t SVO> class Deque<T, SVO>::BwdIterR: public BwdIterW {
    using BwdIterW ::BwdIterW;

public:
    BwdIterR(const BwdIterW& in): // writable -> readonly
        BwdIterW(in) { }
    BwdIterR(const FwdIterR& in): // forward -> backward
        BwdIterW(in) { }
    const T& operator*() { return BwdIterW::operator*(); }   //!< non-const hide
    const T* operator->() { return BwdIterW::operator->(); } //!< non-const hide
};

REGISTER_CONTAINER(Deque, STL_DEQUE);

} // namespace stl
LWE_END
#include "deque.ipp"
#endif
