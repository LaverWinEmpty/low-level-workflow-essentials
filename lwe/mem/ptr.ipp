#ifdef LWE_MEM_PTR

LWE_BEGIN 
namespace mem {

template<typename T> bool Ptr<T>::initialize(bool flag) {
    tracker = static_cast<Tracker*>(std::malloc(sizeof(Tracker)));
    // allocate failed
    if(!tracker) {
        return false;
    }

    // arg == ptr, store external
    if(flag) {
        block = std::malloc(sizeof(External));
        // allocate failed
        if(!external) {
            free(tracker);
            return false;
        }
        external->head = nullptr; // init
    }
    
    // arg != ptr, store internal
    else {
        block = std::malloc(sizeof(Internal));
        // allocate failed
        if(!internal) {
            free(tracker);
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
        external->ptr->~T();
    }
    else internal->data.~T();

    free(block); // union
    block = nullptr; // for safe
    return true;
}

// default
template<typename T> Ptr<T>::Ptr(): pointer(true) {
    // set nullptr
    tracker = nullptr;
    block   = nullptr; // union
}

// pointer
template<typename T> Ptr<T>::Ptr(T* in): pointer(true) {
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
template<typename T> Ptr<T>::Ptr(const T& in): pointer(false) {
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
template<typename T> Ptr<T>::Ptr(T&& in): pointer(false) {
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
template<typename... Args> Ptr<T>::Ptr(Args&&... in): pointer(false) {
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

        // free
        free(tracker);
    }
}

// copy
template<typename T> Ptr<T>::Ptr(const Ptr& in): pointer(in.pointer) {
    // set nullptr
    if(in.tracker == nullptr) {
        tracker = nullptr;
        block   = nullptr;
    }

    // shallow: to weak ptr
    else {
        tracker = static_cast<Tracker*>(std::malloc(sizeof(Tracker)));
        // allocate failed
        if(!tracker) {
            tracker = nullptr;
            throw diag::error(diag::Code::BAD_ALLOC);
        }

        // shared block (union)
        block = in.block;

        // insert this
        push();
    }
}

// copy
template<typename T> auto Ptr<T>::operator=(const Ptr& in) -> Ptr&{
    pointer = in.pointer;

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
            free(tracker); // delete
        }
        tracker = nullptr;
        block   = nullptr;
        return *this;
    }

    // copy
    else {
        // if nullptr
        if(!tracker) {
            tracker = static_cast<Tracker*>(std::malloc(sizeof(Tracker)));
            // allocate failed
            if(!tracker) {
                tracker = nullptr;
                throw diag::error(diag::Code::BAD_ALLOC);
            }
        }

        // shared block (union)
        block = in.block;

        // insert this
        push();
    }
    return *this;
}

// move
template<typename T> auto Ptr<T>::operator=(Ptr&& in) noexcept-> Ptr&{
    if (this != &in) {
        pointer = in.pointer;

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
            free(tracker); // delete
        }

        // move
        tracker    = in.tracker;
        block      = in.block;
        in.tracker = nullptr;
        in.block   = nullptr;
    }
    return *this;
}

// move
template<typename T> Ptr<T>::Ptr(Ptr&& in) noexcept : pointer(in.pointer), tracker(in.tracker), block(in.block) {
    // move
    in.tracker = nullptr;
    in.block   = nullptr; // union
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