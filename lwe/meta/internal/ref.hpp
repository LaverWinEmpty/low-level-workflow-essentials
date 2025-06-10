#ifndef LWE_META_REF
#define LWE_META_REF

#include "../../diag/diag.h"
#include "../../mem/ptr.hpp"

LWE_BEGIN
namespace meta {

//! @brief Object reflection / reference pointer
//! @note  copy is weak_ptr only, set unique -> call clone()
//! @tparam T dervied class of `Object`
template<typename T> class Ref {
    template<typename U> friend class Ref;

public:
    Ref();
    Ref(const Ref&);
    Ref(Ref&&) noexcept;
    template<typename U> Ref(const Ref<U>&);
    template<typename U> Ref(Ref<U>&&);
    ~Ref();

public:
    Ref& operator=(const Ref&);
    Ref& operator=(Ref&&) noexcept;
    template<typename U> Ref& operator=(const Ref<U>&);
    template<typename U> Ref& operator=(Ref<U>&&);

public:
    template<typename U> diag::Expected<Ref<U>> cast();     //!< safe return Ref<U>
    bool                                        clone();    //!< set unique
    T&                                          as();       //!< get reference
    const T&                                    as() const; //!< get reference const

public:
    T*       operator->()       { return reinterpret_cast<T*>(&*ptr); }
    const T* operator->() const { return reinterpret_cast<const T*>(&*ptr); }
    T&       operator*()        { return reinterpret_cast<T*>(*ptr); }
    const T& operator*() const  { return reinterpret_cast<const T*>(*ptr); }

public:
    operator T*();
    operator const T*() const;

private:
    mem::Ptr<Object> ptr;
};

}
LWE_END
#include "ref.ipp"
#endif