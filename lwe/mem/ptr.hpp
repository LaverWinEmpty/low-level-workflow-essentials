#ifndef LWE_MEM_PTR
#define LWE_MEM_PTR

#include "../base/base.h"
#include "../diag/diag.h"

LWE_BEGIN
namespace mem {

struct Tracker {
    Tracker* next, *prev;
};

template<typename T> class Ptr {
    struct Internal {
        ~Internal() { /* not work */ }
        union {
            T       data;
            uint8_t ptr[sizeof(T)] = { 0 }; // memset
        };
        Tracker* head = nullptr;
    };
    struct External {
        T*       ptr  = nullptr;
        Tracker* head = nullptr;
    };

public:
    using Deleter = std::function<void(void*)>;

private:
    bool initialize(bool); // malloc
    bool release();        // free

public:
    //! @param [in] T* exnernal pointer
    //! @param [in] Deleter destructor lambda: default delete in, ignore nullptr
    Ptr(T*, Deleter = [](void* in) { delete in; });

public:
    Ptr();         //!< set nullptr
    Ptr(const T&); //!< data is stored in the control block.
    Ptr(T&&);      //!< data is moved in the control block.

public:
    //! @brief create data in control block (like make_shared<T>)
    template<typename... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
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

public:
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
    Tracker*&      list();
    const Tracker* list() const;

private:
    Tracker* tracker = nullptr;

private:
    union {
        void*     block = nullptr; //!< copy, move, delete
        Internal* internal;
        External* external;
    };

public:
    Deleter deleter; //!< @brief custom deleter

private:
    bool pointer;
};

}
LWE_END
#include "ptr.ipp"
#endif