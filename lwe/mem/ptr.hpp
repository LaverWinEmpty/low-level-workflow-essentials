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

private:
    bool initialize(bool); // malloc
    bool release();        // free

public:
    Ptr();                                     //!< set nullptr
    Ptr(T*);                                   //!< external poitner (new T)
    Ptr(const T&);                             //!< data is stored in the control block.
    Ptr(T&&);                                  //!< data is moved in the control block.
    template<typename... Args> Ptr(Args&&...); //!< create data in control block

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

private:
    bool pointer;
};

}
LWE_END
#include "ptr.ipp"
#endif