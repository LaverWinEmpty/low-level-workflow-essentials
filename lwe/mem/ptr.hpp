#ifndef LWE_MEM_PTR
#define LWE_MEM_PTR

#include "../base/base.h"
#include "../diag/diag.h"
#include "allocator.hpp"

/**************************************************************************************************
 * smart pointer
 *
 * it works like shared_ptr based on unique_ptr with weak_ptr
 * because of this, there is no thread safety.
 * 
 **************************************************************************************************
 * structure
 *
 *   +---------+ +---------+ +---------+         +---------+
 *   | Ptr<A>  | | Ptr<A>  | | Ptr<A>  | < API > | Ptr<B>  |
 *   +---------+ +---------+ +---------+         +---------+
 *        |           |           |                   |
 *   +---------+ +---------+ +---------+         +---------+
 *   | Tracker | | Tracker | | Tracker |  < ID > | Tracker |
 *   +---------+ +---------+ +---------+         +---------+
 *        |           |           |                   | 
 *        +-----------+-----------+              +---------+ 
 *                    |                 unique > |  Block  |
 *          +---------+---------+                +---------+
 *          |  Block + internal | < shared            |
 *          +-------------------+                +----------+
 *                                      unique > | external |
 *                                               +----------+
 *
 **************************************************************************************************
 * Constructor
 *
 * `Ptr()`
 *  - set nullptr
 *
 * `Ptr(const T&)`, `Ptr(T&&)`, `Ptr(Args...)`
 *  - is create T and stored internally
 *  - new `Tracker` for unique ID
 *
 * `Ptr(T*, std::function<void(void*)>)` 
 *  - is get T* and stored externally
 *  - call if function exists, otherwise ignore.
 *  - function is customizable, default [](void* in) { delete in; }
 *  - new `Tracker` for unique ID
 *
 * `Ptr(const Ptr&)`
 *  - copy, shared
 *  - new `Tracker` for unique ID
 *
 * `Ptr(Ptr&&)`
 *  - move
 *  - use existing `Tracker` address
 **************************************************************************************************/

LWE_BEGIN
namespace mem {

struct Tracker {
    Tracker* next, *prev;
};

template<typename T> class Ptr {
    // custom destructor
    using Deleter = std::function<void(void*)>;

    // constrcutor SFINAE for incomplete type
    template<typename U, typename... Args> using Enable = std::enable_if_t<std::is_constructible_v<U, Args...>>;

    //! block base
    struct Block {
        Tracker* head  = nullptr;
        size_t   count = 0;
    };

    //! internal data control block
    struct Internal : Block {
        ~Internal() { /* not work */ }
        union {
            T       data;
            uint8_t ptr[sizeof(T)] = { 0 }; // memset
        };
;
    };

    //! external data control block
    struct External : Block {
        T* ptr = nullptr;
    };

private:
    bool initialize(bool); // malloc
    bool release();        // free


public:
    //! default set nullptr
    Ptr();

public:
    //! @param [in] T* exnernal pointer
    //! @param [in] Deleter destructor lambda: default delete in, ignore nullptr
    Ptr(T*, Deleter = [](void* in) { delete in; });

public:
    //! @brief copy const
    //! @param [in] U is T, for incomplete type
    template<typename U, typename = Enable<T, const U&>>
    Ptr(const U&);

public:
    //! @brief move 
    //! @param [in] U is T, for incomplete type
    template<typename U, typename = Enable<T, U&&>>
    Ptr(U&&);

public:
    //! @brief crete (like a make_shared<T>)
    template<typename... Args, typename = Enable<T, Args>>
    Ptr(Args&&...); 

public:
    ~Ptr();
    Ptr(const Ptr&);                //!< shallow copy 
    Ptr(Ptr&&) noexcept;            //!< move
    Ptr& operator=(const Ptr&);     //!< shallow copy
    Ptr& operator=(Ptr&&) noexcept; //!< move

public:
    T*       operator->();       //!< get ptr
    const T* operator->() const; //!< get ptr const
    T&       operator*();        //!< get ref
    const T& operator*() const;  //!< get ref const

public:
    bool operator==(void*) const; //!< compare to pointer
    bool operator!=(void*) const; //!< compare to pointer
    bool operator==(const Ptr&) const; //!< compare if the block ​​are the same
    bool operator!=(const Ptr&) const; //!< compare if the block ​​are the same

public:
    explicit operator bool() const; //! check nullptr
    operator T*();                  //! raw pointer
    operator const T*() const;      //! raw pointer const

public:
    //! @brief to unique, NEED: copy constructor
    //! @return false: bad alloc
    bool clone();

private:
    //! @return false: block is nullptr
    bool push();

private:
    //! @return false: call on empty
    bool pop();

public:
    bool unique() const;
    bool shared() const;

private:
    T*       data();         //!< get
    const T* data() const;   //!< get

public:
    size_t count() const;

private:
    Tracker* tracker = nullptr;

private:
    Block* block = nullptr;

private:
    Deleter deleter; //!< @brief custom deleter

private:
    bool pointer;
};

}
LWE_END
#include "ptr.ipp"
#endif