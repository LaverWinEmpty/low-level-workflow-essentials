#ifndef LWE_MEM_PTR
#define LWE_MEM_PTR

#include "C:/Git/working/source/base/base.h"

LWE_BEGIN
namespace mem {

//! @brief non-thread-safe unique_ptr with weak-copy semantics.
//! @note  call clone() to make this instance independent.
template<typename T> class Ptr {
    struct Handle {
        Handle* prev = nullptr;
        Handle* next = nullptr;
    };

private:
    struct Handler {
        ~Handler() { /* not work */ }
        union {
            T       data;
            uint8_t ptr[sizeof(T)] = { 0 }; // memset
        };
        Handle* head = nullptr;

    public:
        bool unique() const;

    public:
        void link(Handle*);

    public:
        void unlink(Handle* in);
    };

public:
    //! @brief constructor
    template<typename... Args> Ptr(Args&&...);

public:
    Ptr(const Ptr&);

public:
    Ptr(Ptr&&) noexcept;

public:
    ~Ptr();

public:
    bool clone();

public:
    T*       operator->();
    const T* operator->() const;
    T&       operator*();
    const T& operator*() const;

private:
    Handler* handler;
    Handle* handle;
};

}
LWE_END
#endif