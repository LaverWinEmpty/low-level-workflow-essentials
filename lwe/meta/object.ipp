#include <type_traits>
LWE_BEGIN
namespace meta {

// clang-format off
// for serialize

async::Lock Object::lock;

// feauter.ipp implementation
template<typename T> Registered registclass() {
   // default, other class -> template specialization
   Structure::reflect<Object>();
   Registry<Object>::add<Object>("Object");
   Registry<Class>::add<ObjectMeta>("Object");
   return Registered::REGISTERED;
}

/*
 * object methods
 */
template<typename T> bool Object::isof() const {
    if(this == nullptr) {
        return false;
    }

    static const Class* cls = classof<T>();
    if(cls) {
        const Class* self = meta();
        while(self) {
            if(cls == self) {
                return true;
            }
            self = self->base();
        }
    }
    return false;
}

bool Object::isof(const Class* in) const {
    const Class* self = meta();
    while(self) {
        if(in == self) {
            return true;
        }
        self = self->base();
    }
    return false;
}

bool Object::isof(const char* in) const {
    return isof(String{ in });
}

bool Object::isof(const String& in) const {
    const Class* cls = classof(in);
    if(cls) {
        const Class* self = meta();
        while(self) {
            if(cls == self) {
                return true;
            }
            self = self->base();
        }
    }
    return false;
}

Class* Object::meta() const {
    return classof<Object>();
}

/*
 * object meta source
 */

const char* ObjectMeta::name() const {
    return "Object";
}

size_t ObjectMeta::size() const {
    return sizeof(Object);
}

const Structure& ObjectMeta::fields() const {
    return Structure::reflect<Object>();
}

const Class* ObjectMeta::base() const {
    return nullptr;
}

const Object* ObjectMeta::statics() const {
    return meta::statics<Object>();
}

Object* ObjectMeta::construct(Object* in) const {
    new (in) Object();
    return in;
}


Registered Object_REGISTERED = registclass<Object>();

/*
 * etc
 */

// use metaclass
Object* Object::constructor(const Class* in) {
    void* ptr = nullptr;
    LOCKGUARD(Object::lock) {
        // get size
        size_t size = in->size();

        // size to hash: if same size, use the same pool.
        auto result = Object::pool().find(size);
        // find pool
        if (result == Object::pool().end()) {
            // generate pool
            Object::pool()[size] = new mem::Pool(size);
            // refind
            result = Object::pool().find(size);
        }
        // allocate
        ptr = result->second->allocate<void>();
    }

    // allocate succeeded -> call constructor virtual
    if(ptr) {
        in->construct(static_cast<Object*>(ptr));
    }
    return static_cast<Object*>(ptr);
}

// template
template<typename T> T* Object::constructor() {
    if constexpr(!std::is_base_of_v<Object, T>) {
        return nullptr;
    }

    void* ptr = nullptr;
    LOCKGUARD(Object::lock) {
        // get size
        size_t size = classof<T>()->size();

        // size to hash: if same size, use the same pool.
        auto result = Object::pool().find(size);
        // find pool
        if(result == Object::pool().end()) {
            // generate pool
            Object::pool()[size] = new mem::Pool(size);
            // refind
            result = Object::pool().find(size);
        }
        // allocate
        ptr = result->second->allocate<void>();
    }

    // allocate succeeded -> call constructor T
    if(ptr) {
        if(std::is_copy_constructible_v<T>) {
            new (ptr) T(*statics<T>()); // call copy constructor by default instance
        }
        else new (ptr) T(); // call default constructor
    }
    return static_cast<T*>(ptr);
}

// use metaclass
void Object::destructor(Object* in) {
    if(in == nullptr) {
        return;
    }

    // get size to find pool
    size_t size = in->meta()->size();
    in->~Object();

    LOCKGUARD(Object::lock) {
        // size to hash, and find
        Object::pool()[size]->deallocate<void>(in);
    }
}

// template
template<typename T> void Object::destructor(T* in) {
    if constexpr(!std::is_base_of_v<Object, T>) {
        // TODO:
        // assert(false);
        return;
    }
    
    if(!in) {
        return;
    }

    // get size to find pool
    size_t size = in->meta()->size();
    in->~T();

    // lock
    LOCKGUARD(Object::lock) {
        // size to hash, and find
        Object::pool()[size]->deallocate<void>(in);
    }
}

}
LWE_END

// clang-format on
