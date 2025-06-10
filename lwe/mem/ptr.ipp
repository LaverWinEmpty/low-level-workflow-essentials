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
        block = std::malloc(sizeof(External));
        // allocate failed
        if(!external) {
            // free(tracker)
            Allocator<Tracker>::deallocate(tracker);
            tracker = nullptr; // restore
            return false;
        }
        external->head = nullptr; // init
    }
    
    // arg != ptr, store internal
    else {
        block = std::malloc(sizeof(Internal));
        // allocate failed
        if(!internal) {
            // free(tracker)
            Allocator<Tracker>::deallocate(tracker);
            tracker = nullptr; // restore
            return false;
        }
        internal->head = nullptr; // init
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
            deleter(external->ptr);
        }
        else delete external->ptr; // default delete
    }
    else internal->data.~T();

    free(block); // union
    block   = nullptr; // for safe
    deleter = nullptr; // for safe
    return true;
}

// default
template<typename T> Ptr<T>::Ptr(): pointer(true), deleter(nullptr) {
    // set nullptr
    tracker = nullptr;
    block   = nullptr; // union
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
    // insert
    push();
    // set
    external->ptr = in;
}

// reference
template<typename T> Ptr<T>::Ptr(const T& in): pointer(false), deleter(nullptr) {
    // init
    if(!initialize(false)) {
        // failed
        tracker = nullptr;
        block   = nullptr;
        throw diag::error(diag::Code::BAD_ALLOC);
    }
    // insert
    push();
    // set
    new (&internal->data) T(in);
}

// move
template<typename T> Ptr<T>::Ptr(T&& in): pointer(false), deleter(nullptr) {
    // init
    if(!initialize(false)) {
        // failed
        tracker = nullptr;
        block   = nullptr;
        throw diag::error(diag::Code::BAD_ALLOC);
    }
    // insert
    push();
    // set
    new (&internal->data) T(std::move(in));
}

// T constructor
template<typename T>
template<typename... Args, typename> Ptr<T>::Ptr(Args&&... in): pointer(false), deleter(nullptr) {
    // init
    if(!initialize(false)) {
        // failed
        tracker = nullptr;
        block   = nullptr;
        throw diag::error(diag::Code::BAD_ALLOC);
    }
    // insert
    push();
    // set
    new (&internal->data) T(std::forward<Args>(in)...);
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

        // shared block (union)
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
    in.block   = nullptr; // union
}

// copy
template<typename T> auto Ptr<T>::operator=(const Ptr& in) -> Ptr&{
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

        // shared block (union)
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
    if (this != &in) {
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
        pointer    = in.pointer;
        deleter    = in.deleter;
        tracker    = in.tracker;
        block      = in.block;

        // move
        in.tracker = nullptr;
        in.block   = nullptr;
    }
    return *this;
}

// getter: ptr
template<typename T> T* Ptr<T>::operator->() {
    if(pointer) {
        return external->ptr;
    }
    else return &internal->data;
}

// const getter: ptr
template<typename T> const T* Ptr<T>::operator->() const {
    if(pointer) {
        return external->ptr;
    }
    else return &internal->data;
}

// getter: ref
template<typename T> T& Ptr<T>::operator*() {
    if(pointer) {
        return *external->ptr;
    }
    else return internal->data;
}

// const getter: ref
template<typename T> const T& Ptr<T>::operator*() const {
    if(pointer) {
        return *external->ptr;
    }
    else return internal->data;
}

template<typename T> bool Ptr<T>::operator==(void* in) const {
    if(pointer) {
        return external->ptr == in;
    }
    else return &internal->data == in;
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
    // impossible
    if(unique() || tracker == nullptr) {
        return true;
    }

    Internal* temp = static_cast<Internal*>(std::malloc(sizeof(Internal)));
    // allocated failed
    if(!temp) {
        return false;
    }
    temp->head = nullptr;
    
    // deep copy by placement new copy constructor
    if(pointer) {
        new (&temp->data) T(*external->ptr);
    }
    else new (&temp->data) T(internal->data);

    // pop
    pop();
    // set
    internal = temp;
    pointer  = false;
    // push
    push(); // isnert to new block

    return true;
}

// block management
template<typename T> bool Ptr<T>::push() {
    // check has data
    if(tracker == nullptr) {
        return false;
    }

    // get head
    Tracker*& head = list();

    // push front
    tracker->prev        = nullptr; // [null]<--[this]-->[????]
    tracker->next        = head;    // [null]<--[this]-->[head]
    if (head) head->prev = tracker; // [null]<->[this]<->[head]
    head                 = tracker; // [null]<->[head]<->[....]

    return true;
}

// block management
template<typename T> bool Ptr<T>::pop() {
    if (block == nullptr) {
        return false;
    }

    // get head
    Tracker*& head = list();

    // size 0
    if(head == nullptr) {
        return false;
    }

    // pop front
    if (tracker == head) { // [head]<->[....]<->[....]
        head = head->next; // [this]<->[head]<->[....]
    }

    // pop
    if (tracker->prev) tracker->prev->next = tracker->next; // [prev]<--[ in ]-->[next] 
    if (tracker->next) tracker->next->prev = tracker->prev; //    ^----------------^

    // for safe
    tracker->prev = nullptr; // [prev]   [ in ]-->[next]
    tracker->next = nullptr; // [prev]<---------->[next]

    return true;
}

template<typename T> bool Ptr<T>::unique() const {
    // get head
    const Tracker* head = list();

    // check
    if(tracker) {
        return head->next == nullptr;
    }
    return false;
}

template<typename T> bool Ptr<T>::shared() const {
    // get head
    const Tracker* head = list();

    // check
    if(tracker) {
        return head->next != nullptr;
    }
    return false;
}

template<typename T> Tracker*& Ptr<T>::list() {
    return pointer ? external->head : internal->head;
}

template<typename T> const Tracker* Ptr<T>::list() const {
    return pointer ? external->head : internal->head;
}

}
LWE_END
#endif