#ifndef LWE_CONTAINER_DEQUE_HEADER
#define LWE_CONTAINER_DEQUE_HEADER

/**************************************************************************************************
 * circulation array
 *
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
 *
 * push_back: push
 * pop_back: pop
 * push_front: unshift
 * pop_front: shift
 **************************************************************************************************/

#include <cstdlib>
#include <stdexcept>
#include "common.hpp"
#include "container.hpp"
#include "serialize.hpp"

LWE_BEGIN

// TODO:

namespace stl {

template<typename T, size_t SVO = DEF_SVO> struct Deque: Container {
    DECLARE_CONTAINER(Deque, T);

private:
    template<typename, size_t> friend struct Deque;

private:
    static constexpr size_t MIN = SVO < DEF_SVO ? DEF_SVO : Common::align(SVO);

public:
    class Iterator; //!< iterator
    class Reverser; //!< reverse iterator
    friend class Iterator;
    friend class Reverser;

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
    size_t capacity() const noexcept; //!< array size
    bool   full() const noexcept;
    bool   empty() const noexcept;

public:
    T* data() const noexcept; //!< array address
    T& at(index_t) const;

public:
    Iterator begin() const noexcept;  //!< iterator begin
    Iterator end() const noexcept;    //!< iterator end
    Reverser rbegin() const noexcept; //!< reverse iterator begin
    Reverser rend() const noexcept;   //!< reverse iterator end
    Iterator front() const noexcept;  //!< iterator for head to tail
    Reverser rear() const noexcept;   //!< iterator for tail to head
    Reverser top() const noexcept;    //!< iterator for tail to head
    Iterator bottom() const noexcept; //!< iterator for head to tail

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
        T stack[MIN]; //!< small
    };
    T*      container = stack; //!< array
    size_t  capacitor = MIN;   //!< size: array
    index_t counter   = 0;     //!< size: element
    index_t head      = 0;     //!< index: front / bottom
    index_t tail      = -1;    //!< index: rear / top
};

template<typename T, size_t SVO> class Deque<T, SVO>::Iterator {
    friend class Reverser;
public:
    Iterator(const Deque*, index_t) noexcept;
    Iterator(const Iterator&) noexcept;
    Iterator(const Reverser&) noexcept;
    Iterator& operator=(const Iterator&) noexcept;
    Iterator& operator=(const Reverser&) noexcept;
    Iterator& operator++() noexcept;
    Iterator& operator--() noexcept;
    Iterator  operator++(int) noexcept;
    Iterator  operator--(int) noexcept;
    Iterator  operator+(index_t) const noexcept;
    Iterator  operator-(index_t) const noexcept;
    Iterator& operator+=(index_t) noexcept;
    Iterator& operator-=(index_t) noexcept;
    bool operator==(const Iterator&) const noexcept;
    bool operator!=(const Iterator&) const noexcept;
    bool operator==(const Reverser&) const noexcept;
    bool operator!=(const Reverser&) const noexcept;
    bool operator<(const Iterator&) const noexcept;
    bool operator>(const Iterator&) const noexcept;
    bool operator<=(const Iterator&) const noexcept;
    bool operator>=(const Iterator&) const noexcept;
    bool operator<(const Reverser&) const noexcept;
    bool operator>(const Reverser&) const noexcept;
    bool operator<=(const Reverser&) const noexcept;
    bool operator>=(const Reverser&) const noexcept;
    T&   operator*() const noexcept;
    T*   operator->() const noexcept;
private:
    const Deque* outer;
    index_t      index;
};

template<typename T, size_t SVO> class Deque<T, SVO>::Reverser {
    friend class Iterator;
public:
    Reverser(const Deque*, index_t) noexcept;
    Reverser(const Reverser&) noexcept;
    Reverser(const Iterator&) noexcept;
    Reverser& operator=(const Reverser&) noexcept;
    Reverser& operator=(const Iterator&) noexcept;
    Reverser& operator++() noexcept;
    Reverser& operator--() noexcept;
    Reverser  operator++(int) noexcept;
    Reverser  operator--(int) noexcept;
    bool      operator==(const Reverser&) const noexcept;
    bool      operator!=(const Reverser&) const noexcept;
    bool      operator==(const Iterator&) const noexcept;
    bool      operator!=(const Iterator&) const noexcept;
    bool      operator<(const Reverser&) const noexcept;
    bool      operator>(const Reverser&) const noexcept;
    bool      operator<=(const Reverser&) const noexcept;
    bool      operator>=(const Reverser&) const noexcept;
    bool      operator<(const Iterator&) const noexcept;
    bool      operator>(const Iterator&) const noexcept;
    bool      operator<=(const Iterator&) const noexcept;
    bool      operator>=(const Iterator&) const noexcept;
    Reverser  operator+(index_t) const noexcept;
    Reverser  operator-(index_t) const noexcept;
    Reverser& operator+=(index_t) noexcept;
    Reverser& operator-=(index_t) noexcept;
    T& operator*() const noexcept;
    T* operator->() const noexcept;
private:
    Iterator iterator;
};

LWE_END

} // namespace stl
#include "deque.ipp"
#endif