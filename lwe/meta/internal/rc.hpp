#ifndef LWE_META_RC
#define LWE_META_RC

#include "../../diag/diag.h"
#include "../../mem/ptr.hpp"

LWE_BEGIN
namespace meta {

//! @brief  `R`eflect `C`lass
//! @note   copy is weak_ptr only, set unique -> call clone()
//! @tparam T dervied class of `Object`
template<typename T> class RC {
    template<typename U> friend class RC;

public:
    RC();
    RC(const RC&);
    RC(RC&&) noexcept;
    template<typename U> RC(const RC<U>&);
    template<typename U> RC(RC<U>&&);
    RC(std::nullptr_t);
    ~RC();

public:
    RC&                      operator=(const RC&);
    RC&                      operator=(RC&&) noexcept;
    template<typename U> RC& operator=(const RC<U>&);
    template<typename U> RC& operator=(RC<U>&&);

public:
    template<typename U> U* cast();      //!< safe cast
    template<typename U> U* as();        //!< unsafe cast
    bool                    clone();     //!< set unique
    T*                      get();       //!< get RCerence
    const T*                get() const; //!< get RCerence const

public:
    bool valid() const; //!< check nullptr or dangling
    bool owned() const; //!< check owner
    bool own();         //!< set owner

public:
    T*       operator->() { return reinterpret_cast<T*>(&*ptr); }
    const T* operator->() const { return reinterpret_cast<const T*>(&*ptr); }
    T&       operator*() { return reinterpret_cast<T*>(*ptr); }
    const T& operator*() const { return reinterpret_cast<const T*>(*ptr); }

public:
    explicit operator bool() const;

private:
    mem::Ptr<Object> ptr;
};

} // namespace meta
LWE_END
#include "rc.ipp"
#endif
