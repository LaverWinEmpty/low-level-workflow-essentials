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
    bool initialize(bool);
    bool release();

public:
    Ptr();
    Ptr(T*);
    Ptr(const T&);
    Ptr(T&&);
    template<typename... Args> Ptr(Args&&...);

public:
    ~Ptr();
    Ptr(const Ptr&);
    Ptr(Ptr&&) noexcept;
    Ptr& operator=(const Ptr&);
    Ptr& operator=(Ptr&&) noexcept;

public:
    T*       operator->();
    const T* operator->() const;
    T&       operator*();
    const T& operator*() const;

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
    Tracker* tracker;

private:
    union {
        void*     block; //!< copy, move, delete
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