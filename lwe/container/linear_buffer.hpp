/*
    Linear buffer class

    API
    - push(T)
    - pop(T& or T*, nullable)
    - insert(index, T)
    - remove(index, T*)

    ! SWAP AND DELETE / INSERT !
    e.g.
    +-----------------+
    | [0][1][2][ ][ ] |
    +-----------------+
    insert(1, T{ 3 });
    +-----------------+
    | [0][3][2][1][ ] |
    |           ^swap |
    +-----------------+
    remove(0);
    +-----------------+
    | [1][3][2][ ][ ] |
    |  ^swap          |
    +-----------------+
*/

#ifndef LWE_CONTAINER_LINEAR_BUFFER
#define LWE_CONTAINER_LINEAR_BUFFER

#include "../config/config.h"
#include "../mem/block.hpp"
#include "iterator.hpp"

LWE_BEGIN
namespace container {

//! @tparam N count of T, 0 is auto size (64 byte)
template<typename T, size_t SVO = 0>
class LinearBuffer {
    template<typename, size_t> friend class LinearBuffer; //!< for LinearBuffer<T, OTHER_SVO_SIZE>
    template<typename, size_t> friend class RingBuffer;   //!< for composition

public:
    CONTAINER_BODY(LinearBuffer, T, T, SVO);

private:
    static constexpr size_t min() {
        constexpr size_t ALIGNED = align(config::SMALLVECTOR / sizeof(T));
        return ALIGNED < 4 ? 0 : ALIGNED;
    }

public:
    static constexpr size_t MIN = SVO ? align(SVO) : min();

public:
    LinearBuffer();
    LinearBuffer(const LinearBuffer&);
    LinearBuffer(LinearBuffer&&) noexcept;
    LinearBuffer& operator=(const LinearBuffer&);
    LinearBuffer& operator=(LinearBuffer&&) noexcept;
    ~LinearBuffer();

public:
    template<size_t N> LinearBuffer(const LinearBuffer<T, N>&);
    template<size_t N> LinearBuffer(LinearBuffer<T, N>&&) noexcept;
    template<size_t N> LinearBuffer<T, N>& operator=(const LinearBuffer<T, N>&);
    template<size_t N> LinearBuffer<T, N>& operator=(LinearBuffer<T, N>&&) noexcept;

public:
    T& operator[](index_t) noexcept;             //!<
    const T& operator[](index_t) const noexcept; //!<

public:
    bool push() noexcept;         //!< push_back default constructor
    bool push(T&&) noexcept;      //!< push_back
    bool push(const T&) noexcept; //!< push_back

public:
    bool pop(T* = nullptr) noexcept; //!< pop_back
    bool pop(T&) noexcept;           //!< pop_back

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
    template<typename Arg> bool emplace(index_t, Arg&&) noexcept; //!< out of range: push_back or push_front
    bool                        erase(index_t, T* = nullptr) noexcept;


private:
    template<size_t X, bool COPY> using Other =
        std::conditional_t<COPY, const LinearBuffer<T, X>&, LinearBuffer<T, X>&&>;
    template<size_t X, bool COPY> bool ctor(Other<X, COPY>, index_t = 0);           //!< copy / move delegator
    template<bool> void                transfer(const T*, T*, index_t, size_t = 0); //!< true: copy, false: move
    bool                               reallocate(size_t, index_t = 0);             //!< call realloc (size, begin)

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
#include "linear_buffer.ipp"
#endif
