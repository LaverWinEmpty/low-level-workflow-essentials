#ifndef LWE_CONTAINER_DEQUE
#define LWE_CONTAINER_DEQUE

#include "stack.hpp"

/*
    Deque
    - Ring buffer class

    API
    - push(T)    push back
    - pop(T&)    LIFO (T& or T*, nullable)
    - pull(T&)   FIFO (T& or T*, nullable)
    - prepend(T) push front

    - insert(index, T)
    - remove(index, T*)
    - operator[](index)
    relative index
    e.g.
    +--------------------------+
    | [d][e][ ][ ][ ][a][b][c] |
    +--------------------------+
    | begin    5               |
    | size     5               |
    | capacity 8               |
    +--------------------------+
    in: 0 -> process [7] ... a
    in: 2 -> process [7] ... c
    in: 4 -> process [2] ... e
    in: 7 -> process [4] ... out of range
    in: 8 -> out of range

    ! SWAP AND DELETE / INSERT !
    e.g.
    +--------------------+
    | [ ][0][1][2][ ][ ] |
    +--------------------+
    insert(1, T{ 3 });
    +--------------------+
    | [ ][0][3][2][1][ ] |
    |              ^swap |
    +--------------------+
    remove(2);
    +--------------------+
    | [ ][0][3][1][ ][ ] |
    |           ^swap    |
    +--------------------+

    emplace / erase (private)
    absolute index
    0 -> 0
    16 -> 16
    32 -> 32

    iterator
    - absolute index
    ! MODULO OPERATION ON ACCESS !
    0 -> 0
    4 -> 4
    8 -> 0 (8 % 8)    | capacity == 8
    -1 -> 7 (255 % 8) | capacity == 8
*/

LWE_BEGIN
namespace container {
template<typename T, size_t SVO = 0> class Deque {

public:
    CONTAINER_BODY(Deque, T, T, SVO);
    using Stack = Stack<T, SVO>;

public:
    Deque() = default;
    Deque(const Deque&);
    Deque(Deque&&) noexcept;
    Deque& operator=(const Deque&);
    Deque& operator=(Deque&&) noexcept;

public:
    template<size_t X> Deque(const Deque<T, X>&);
    template<size_t X> Deque(Deque<T, X>&&) noexcept;
    template<size_t X> Deque& operator=(const Deque<T, X>&);
    template<size_t X> Deque& operator=(Deque<T, X>&&) noexcept;

public:
    T&       operator[](size_t);
    const T& operator[](size_t) const;

public:
    bool push(const T&);
    bool push(T&&);
    bool push();
    bool pop(T* = nullptr);
    bool pop(T&);

public:
    bool prepend(const T&);
    bool prepend(T&&);
    bool prepend();
    bool pull(T* = nullptr);
    bool pull(T&);

public:
    template<typename Arg> bool insert(index_t, Arg&&);
    bool                        remove(index_t, T&);
    bool                        remove(index_t, T* = nullptr);

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
    Iterator<FWD> begin() noexcept;  //!< iterator begin
    Iterator<FWD> end() noexcept;    //!< iterator end
    Iterator<BWD> rbegin() noexcept; //!< reverse iterator begin
    Iterator<BWD> rend() noexcept;   //!< reverse iterator end

public:
    Iterator<FWD | VIEW> begin() const noexcept;  //!< iterator begin
    Iterator<FWD | VIEW> end() const noexcept;    //!< iterator end
    Iterator<BWD | VIEW> rbegin() const noexcept; //!< reverse iterator begin
    Iterator<BWD | VIEW> rend() const noexcept;   //!< reverse iterator end

public:
    T* top() noexcept;
    T* bottom() noexcept;
    T* front() noexcept;
    T* rear() noexcept;

public:
    const T* top() const noexcept;
    const T* bottom() const noexcept;
    const T* front() const noexcept;
    const T* rear() const noexcept;

public:
    template<typename U> void push_back(U&&);  //!< STL compatible
    void                      pop_back();      //!< STL compatible
    template<typename U> void push_front(U&&); //!< STL compatible
    void                      pop_front();     //!< STL compatible

private:
    template<typename Arg> bool emplace(index_t, Arg&&);
    bool                        erase(index_t, T*);
    bool                        reallocate(size_t);

private:
    index_t absidx(index_t) const noexcept;
    index_t relidx(index_t) const noexcept;

private:
    Stack   stack;
    index_t head = 0; // pos of inserted front
    index_t tail = 0; // pos to insert back
};

} // namespace container
LWE_END
#include "deque.ipp"
#endif
