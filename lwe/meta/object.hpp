#ifndef LWE_OBJECT
#define LWE_OBJECT

#include "type.hpp"
#include "lambda.hpp"

#include "../mem/pool.hpp"
#include "../mem/ptr.hpp"
#include "../diag/diag.h"

LWE_BEGIN
namespace meta {

struct Class; // meta class

template<typename T> class Ref;

/**
 * @brief serializable object: template, array not support
 */
class Object {
    template<typename T> friend class Ref;

protected:
    template<typename T> static T*   constructor();
    template<typename T> static void destructor(T*);
    static Object*                   constructor(const Class*);
    static void                      destructor(Object*);

public:
    virtual ~Object();

public:
    virtual Class* meta() const;

public:
    std::string serialize() const;
    void        deserialize(const std::string& in);
    static void deserialize(Object* out, const std::string& in);

public:
    template<typename T> bool isof() const;              //!< check same type of derived by template
    bool                      isof(const Class*) const;  //!< check same type of derived by meta class
    bool                      isof(const char*) const;   //!< check same type of derived by name
    bool                      isof(const string&) const; //!< check same type of derived by name

private:
    struct Pool {
        ~Pool() {
            for(auto& p : map) delete p.second;
        }
        std::unordered_map<size_t, mem::Pool*> map;
    };
    static std::unordered_map<size_t, mem::Pool*>& pool() {
        static std::unordered_map<size_t, mem::Pool*> instance;
        return instance;
    }
    static async::Lock lock;
};

//! @brief Object Metadata, has not base -> manual generation
struct ObjectMeta: Class {
    virtual const char*      name() const override;
    virtual size_t           size() const override;
    virtual const Structure& fields() const override;
    virtual const Class*     base() const override;
    virtual const Object*    statics() const override;
    virtual Object*          construct(Object*) const override;
};

//! @brief Object pointer
//! @note  Ref means reflection, it also means reference
//!        copy is weak_ptr only, set unique -> call clone()
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
    template<typename U> diag::Expected<Ref<U>> cast(bool = true);
    bool clone();

public:
    T*       operator->()       { return reinterpret_cast<T*>(&*ptr); }
    const T* operator->() const { return reinterpret_cast<const T*>(&*ptr); }

private:
    mem::Ptr<Object> ptr;
};

template<typename T> string serialize(const T&);                    //!< primitive type to string
template<> string           serialize<bool>(const bool&);           //!< boolean type to string
template<> string           serialize<string>(const string&);       //!< string to string
template<> string           serialize<Container>(const Container&); //!< container to string
template<typename T> string serialize(const Container*);            //!< container to string, T is derived type

template<typename T> T    deserialize(const string&);             //!< string to primitive type
template<> bool           deserialize<bool>(const string&);       //!< string to boolean type
template<> string         deserialize<string>(const string&);     //!< string to string
void                      deserialize(Container*, const string&); //!< string to container
template<typename T> void deserialize(Container*, const string&); //!< string to container, T is dervied type

void serialize(string*, const void*, const Keyword&);   //!< runtime serialize
void deserialize(void*, const string&, const Keyword&); //!< runtime deserialize

// ctor: default
template<typename T> Ref<T>::Ref():
    ptr(
        static_cast<Object*>(Object::constructor<T>()),
        // custom deallocator
        [](void* in) {
            Object::destructor(static_cast<Object*>(in));
        } // end lambda
    ) // end ptr()
{}

// copy: shallow
template<typename T> Ref<T>::Ref(const Ref& in): ptr(in.ptr) {}

template<typename T>
template<typename U>
Ref<T>::Ref(const Ref<U>& in) {
    // check failed
    if (!in->isof<T>()) {
        throw diag::error(diag::Code::TYPE_MISMATCH);
    }
    // shallow copy
    ptr = in.ptr;
}

template<typename T> Ref<T>::Ref(Ref&& in) noexcept: ptr(std::move(in.ptr)) {}

template<typename T> template<typename U> Ref<T>::Ref(Ref<U>&& in) {
    if(!in->isof<T>()) {
        throw diag::error(diag::Code::TYPE_MISMATCH);
    }

    ptr = std::move(in.ptr);
}

template<typename T> Ref<T>::~Ref() {}

template<typename T>
template<typename U> diag::Expected<Ref<U>> Ref<T>::cast(bool safe) {
    // checked
    if (safe) {
        if (ptr->isof<U>()) {
            return Ref<U>(*this);
        }
        else return diag::error(diag::Code::TYPE_MISMATCH); // cast failed
    }

    // unchecked
    return Ref<U>(*this);
}

template<typename T>
bool Ref<T>::clone() {
    return ptr.clone();
}

} // namespace meta
LWE_END

#include "object.ipp"
#endif