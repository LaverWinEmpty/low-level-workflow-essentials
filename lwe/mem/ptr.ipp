#ifdef LWE_MEM_PTR

LWE_BEGIN 
namespace mem {

template<typename T> bool Ptr<T>::initialize(bool flag) {
    // malloc(sizeof(Tracker))
    tracker = Allocator<Tracker>::allocate();

    // allocate failed
    if(!tracker) {
        return false;
    }

    // arg == ptr, store external
    if(flag) {
        block = new(std::nothrow) External;
        // allocate failed
        if(!block) {
            // free(tracker)
            Allocator<Tracker>::deallocate(tracker);
            tracker = nullptr; // restore
            return false;
        }
    }
    
    // arg != ptr, store internal
    else {
        block = new(std::nothrow) Internal;
        // allocate failed
        if(!block) {
            // free(tracker)
            Allocator<Tracker>::deallocate(tracker);
            tracker = nullptr; // restore
            return false;
        }
    }

    return true;
}

template<typename T> bool Ptr<T>::release() {
    // set
    if(!block) {
        return false;
    }

    // call destructor
    if(pointer) {
        if(deleter) {
            deleter(get());
        }
        // else delete get(); // default delete X
    }
    else get()->~T();

    delete block;
    block   = nullptr; // for safe
    deleter = nullptr; // for safe
    return true;
}

// default
template<typename T> Ptr<T>::Ptr(): pointer(true), deleter(nullptr) {
    // set nullptr
    tracker = nullptr;
    block   = nullptr;
}

// pointer
template<typename T> Ptr<T>::Ptr(T* in, Deleter func): pointer(true), deleter(func) {
    if(in == nullptr) {
        return;
    }

    // init
    if(!initialize(true)) {
        // failed
        tracker = nullptr;
        block   = nullptr;
        throw diag::error(diag::Code::BAD_ALLOC);
    }
    push(); // insert
    reinterpret_cast<External*>(block)->ptr = in; // set
}

// reference
template<typename T>
template<typename U, typename> Ptr<T>::Ptr(const U& in): pointer(false), deleter(nullptr) {
    // init
    if(!initialize(false)) {
        // failed
        tracker = nullptr;
        block   = nullptr;
        throw diag::error(diag::Code::BAD_ALLOC);
    }
    push(); // insert
    new (reinterpret_cast<Internal*>(block)->ptr) T(in); // copy
}

// move
template<typename T>
template<typename U, typename> Ptr<T>::Ptr(U&& in): pointer(false), deleter(nullptr) {
    // init
    if(!initialize(false)) {
        // failed
        tracker = nullptr;
        block   = nullptr;
        throw diag::error(diag::Code::BAD_ALLOC);
    }
    push(); // insert
    new (reinterpret_cast<Internal*>(block)->ptr) T(std::move(in)); // move
}

// T constructor
template<typename T>
template<typename... Args, typename>
Ptr<T>::Ptr(Args&&... in): pointer(false), deleter(nullptr) {
    // init
    if(!initialize(false)) {
        // failed
        tracker = nullptr;
        block   = nullptr;
        throw diag::error(diag::Code::BAD_ALLOC);
    }
    push(); // insert
    new (get()) T(std::forward<Args>(in)...); // create
}

// dtor
template<typename T> Ptr<T>::~Ptr() {
    // has tracker == has block
    if(tracker) {
        // shared, not free block, pop
        if(shared()) {
            // remove
            pop();
        }

        // unique, free block, not pop
        else release();

        // free(tracker)
        Allocator<Tracker>::deallocate(tracker);
    }
}

// copy
template<typename T> Ptr<T>::Ptr(const Ptr& in): pointer(in.pointer), deleter(in.deleter) {
    // set nullptr
    if(in.tracker == nullptr) {
        tracker = nullptr;
        block   = nullptr;
    }

    // shallow: to weak ptr
    else {
        // malloc(sizeof(Tracker));
        tracker = Allocator<Tracker>::allocate();

        // allocate failed
        if(!tracker) {
            throw diag::error(diag::Code::BAD_ALLOC);
        }

        // shared block
        block = in.block;

        // insert this
        push();
    }
}

// move
template<typename T> Ptr<T>::Ptr(Ptr&& in) noexcept :
    pointer(in.pointer), tracker(in.tracker), block(in.block), deleter(in.deleter) {
    // move
    in.tracker = nullptr;
    in.block   = nullptr;
}

// copy
template<typename T> auto Ptr<T>::operator=(const Ptr& in) -> Ptr& {
    if(this == &in) return *this;

    // tracking
    if(tracker) {
        // last
        if(unique()) {
            release();
        }
        pop();
    }

    // set nullptr
    if(in.tracker == nullptr) {
        if(tracker) {
            // free(tracker)
            Allocator<Tracker>::deallocate(tracker);
        }

        deleter = nullptr;
        tracker = nullptr;
        block   = nullptr;
        return *this;
    }

    // copy
    else {
        // if nullptr
        if(!tracker) {
            // malloc(sizeof(Tracker))
            tracker = Allocator<Tracker>::allocate();

            // allocate failed
            if(!tracker) {
                deleter = nullptr;
                block   = nullptr;
                throw diag::error(diag::Code::BAD_ALLOC);
            }
        }

        // shared block
        block   = in.block;
        deleter = in.deleter;
        pointer = in.pointer;

        // insert this
        push();
    }
    return *this;
}

// move
template<typename T> auto Ptr<T>::operator=(Ptr&& in) noexcept-> Ptr&{
    if(this == &in) return *this;

    // tracking
    if(tracker) {
        // last
        if(unique()) {
            release();
        }
        pop();
    }

    // set nullptr this
    if(tracker) {
        // free(tracker)
        Allocator<Tracker>::deallocate(tracker);
    }

    // move
    pointer = in.pointer;
    deleter = in.deleter;
    tracker = in.tracker;
    block   = in.block;

    // move
    in.tracker = nullptr;
    in.block   = nullptr;
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

template<typename T> Ptr<T>::operator bool () const {
    return tracker != nullptr;
}

template<typename T> Ptr<T>::operator T* () {
    return operator->();
}

template<typename T> Ptr<T>::operator const T* () const {
    return operator->();
}

template<typename T> bool Ptr<T>::clone() {
    // not failed
    if(!block) {
        return true;
    }

    Internal* temp = new(std::nothrow) Internal;
    // allocated failed
    if(!temp) {
        return false;
    }
    temp->head = nullptr;
    
    // deep copy by placement new copy constructor
    new (temp->ptr) T(*get());

    pop();            // pop
    block = temp;     // set
    pointer  = false; // internal
    push();           // isnert to new block

    return true;
}

template<typename T> T* Ptr<T>::get() {
    if (!block) {
        return nullptr;
    }
    if (pointer) {
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

// block management
template<typename T> bool Ptr<T>::push() {
    // check has data
    if(tracker == nullptr) {
        return false;
    }

    // push front
    tracker->prev                      = nullptr;     // [null]<--[this]-->[????]
    tracker->next                      = block->head; // [null]<--[this]-->[head]
    if (block->head) block->head->prev = tracker;     // [null]<->[this]<->[head]
    block->head                        = tracker;     // [null]<->[head]<->[....]


    ++block->count;
    return true;
}

// block management
template<typename T> bool Ptr<T>::pop() {
    if (block == nullptr) {
        return false;
    }

    // size 0
    if(block->head == nullptr) {
        return false;
    }

    // pop front
    if (tracker == block->head) {        // [head]<->[....]<->[....]
        block->head = block->head->next; // [this]<->[head]<->[....]
    }

    // pop
    if (tracker->prev) tracker->prev->next = tracker->next; // [prev]<--[ in ]-->[next] 
    if (tracker->next) tracker->next->prev = tracker->prev; //    ^----------------^

    // for safe
    tracker->prev = nullptr; // [prev]   [ in ]-->[next]
    tracker->next = nullptr; // [prev]<---------->[next]

    --block->count;
    return true;
}

template<typename T> bool Ptr<T>::unique() const {
    if (!block) {
        return false;
    }
    return block->count == 1;
}

template<typename T> bool Ptr<T>::shared() const {
    if (!block) {
        return false;
    }
    return block->count > 1;
}

template<typename T> size_t Ptr<T>::count() const {
    if (!block) {
        return 0;
    }
    return block->count;
}

}
LWE_END
#endif