LWE_BEGIN
namespace mem {

//! specailizated pool for same-sized types
template<size_t N> class Slotmap<Block<N>> {
    static async::Lock lock;
    static Pool        pool;
public:
    static void* acquire();
    static void  release(void* in);
};
template<size_t N> async::Lock Slotmap<Block<N>>::lock;
template<size_t N> Pool        Slotmap<Block<N>>::pool{ N };

template<typename T> T* Slotmap<T>::acquire() {
    return static_cast<T*>(Adapter::acquire());
}

template<typename T> void Slotmap<T>::release(T* in) {
    Adapter::release(static_cast<void*>(in));
}

template<size_t N> void* Slotmap<Block<N>>::acquire() {
    LOCKGUARD(lock) return pool.allocate();
}

template<size_t N> void Slotmap<Block<N>>::release(void* in) {
    LOCKGUARD(lock) pool.deallocate(in);
}

template<typename T> bool Ptr<T>::initialize(bool flag) {
    pointer = flag;
    id      = 0;

    // arg == ptr, store external
    if(flag) {
        block = Slotmap<External>::acquire();
        // acquire failed
        if(!block) {
            return false;
        }
    }

    // arg != ptr, store internal
    else {
        block = Slotmap<Internal>::acquire();
        // acquire failed
        if(!block) {
            return false;
        }
    }

    // succeded
    id = util::ID<Ptr<T>>().value(); // get id

    block->id    = id;   // set block id
    block->owner = this; // set owner
    return true;
}

template<typename T> bool Ptr<T>::release() {
    if(!block) {
        return false;
    }

    // call destructor
    if(block->owner == this) {
        block->id = 0; // defeasance
        if(pointer) {
            // if external call deleter(ptr);
            if(deleter) {
                deleter(get());
            }
            Slotmap<External>::release(reinterpret_cast<External*>(block)); // free
        }
        else {
            // else call dtor only
            get()->~T();
            Slotmap<Internal>::release(reinterpret_cast<Internal*>(block)); // free
        }
    }

    id      = 0;
    block   = nullptr; // for safe
    deleter = nullptr; // for safe
    return true;
}

// default: set nullptr
template<typename T> Ptr<T>::Ptr(): block(nullptr), deleter(nullptr), id(0), pointer(true) { }

// pointer
template<typename T> Ptr<T>::Ptr(T* in, Deleter func): block(nullptr), deleter(nullptr), id(0), pointer(true) {
    if(in == nullptr) {
        return;
    }
    deleter = func; // for safe

    if(!initialize(true)) {
        throw diag::error(diag::Code::BAD_ALLOC); // init failed
    }
    reinterpret_cast<External*>(block)->ptr = in; // set
}

// reference
template<typename T>
template<typename U, typename> Ptr<T>::Ptr(const U& in): deleter(nullptr) {
    if(!initialize(false)) {
        throw diag::error(diag::Code::BAD_ALLOC); // init failed
    }
    new(reinterpret_cast<Internal*>(block)->ptr) T(in); // copy
}

// move
template<typename T>
template<typename U, typename> Ptr<T>::Ptr(U&& in): deleter(nullptr) {
    if(!initialize(false)) {
        throw diag::error(diag::Code::BAD_ALLOC); // iit failed
    }
    new(reinterpret_cast<Internal*>(block)->ptr) T(std::move(in)); // move
}

// dtor
template<typename T> Ptr<T>::~Ptr() {
    release(); // if this is the owner, call free
}

// copy
template<typename T> Ptr<T>::Ptr(const Ptr& in):
    block(in.block),
    deleter(in.deleter),
    id(in.id),
    pointer(in.pointer) { }

// move
template<typename T> Ptr<T>::Ptr(Ptr&& in) noexcept:
    block(in.block),
    deleter(in.deleter),
    id(in.id),
    pointer(in.pointer) {
    // move
    in.block = nullptr;
    if(block->owner == &in) {
        block->owner = this;
    }
}

// copy
template<typename T> auto Ptr<T>::operator=(const Ptr& in) -> Ptr& {
    if(this == &in) return *this;

    // reset
    if(block) {
        release();
    }

    block   = in.block;
    deleter = in.deleter;
    id      = in.id;
    pointer = in.pointer;
    return *this;
}

// move
template<typename T> auto Ptr<T>::operator=(Ptr&& in) noexcept -> Ptr& {
    if(this == &in) return *this;

    // reset
    if(block) {
        release();
    }

    // move
    block   = in.block;
    deleter = in.deleter;
    id      = in.id;
    pointer = in.pointer;

    // move
    in.block = nullptr;

    // block not null
    if(block && block->owner == &in) {
        block->owner = this;
    }
    return *this;
}

// getter: ptr
template<typename T> T* Ptr<T>::operator->() {
    return get();
}

// const getter: ptr
template<typename T> const T* Ptr<T>::operator->() const {
    return get();
}

// getter: ref
template<typename T> T& Ptr<T>::operator*() {
    return *get();
}

// const getter: ref
template<typename T> const T& Ptr<T>::operator*() const {
    return *get();
}

template<typename T> bool Ptr<T>::operator==(void* in) const {
    return get() == in;
}

template<typename T> bool Ptr<T>::operator!=(void* in) const {
    return !operator==(in);
}

template<typename T> bool Ptr<T>::operator==(const Ptr& in) const {
    return block == in.block;
}

template<typename T> bool Ptr<T>::operator!=(const Ptr& in) const {
    return !operator==(in);
}

template<typename T> Ptr<T>::operator bool() const {
    return valid();
}

template<typename T> Ptr<T>::operator T*() {
    return get();
}

template<typename T> Ptr<T>::operator const T*() const {
    return get();
}

template<typename T> bool Ptr<T>::clone() {
    // not failed
    if(!block) {
        return true;
    }

    // is owner
    if(block->owner == this) {
        return true;
    }

    T* data = get(); // pre-get

    if(!initialize(false)) {
        return false; // failed
    }

    // deep copy by placement new copy constructor
    new(get()) T(*data);
    return true;
}

template<typename T> T* Ptr<T>::get() {
    if(!block) {
        return nullptr;
    }
    if(pointer) {
        return reinterpret_cast<External*>(block)->ptr;
    }
    else return &reinterpret_cast<Internal*>(block)->data;
}

template<typename T> const T* Ptr<T>::get() const {
    return const_cast<Ptr*>(this)->get();
}

template<typename T> template<typename U> U* Ptr<T>::as() {
    return reinterpret_cast<U*>(get());
}

template<typename T> template<typename U> const U* Ptr<T>::as() const {
    return reinterpret_cast<U*>(get());
}

template<typename T> bool Ptr<T>::owned() const {
    if(block) {
        return block->owner == this; // same
    }
    return false;
}

template<typename T> void Ptr<T>::own() {
    if(block) {
        block->owner = this;
    }
}

template<typename T> bool Ptr<T>::valid() const {
    if(block && id) {
        return id == block->id; // same and not 0
    }
    return false; // invalid
}

} // namespace mem
LWE_END
