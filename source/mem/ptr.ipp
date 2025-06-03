#ifdef LWE_MEM_PTR

LWE_BEGIN 
namespace mem {

template<typename T> template<typename... Args> Ptr<T>::Ptr(Args&&... in)  {
    // allocate
    handle  = static_cast<Handle*>(std::malloc(sizeof(Handle)));
    handler = static_cast<Handler*>(std::malloc(sizeof(Handler)));

    // check
    if (handle == nullptr || handler == nullptr) {
        if (handle) {
            std::free(handle);
            handle = nullptr;
        }
        if (handler) {
            std::free(handler);
            handler = nullptr;
        }
        throw std::bad_alloc(); // failed
    }

    // init
    else {
        new (handler) Handler();
        new (handle)  Handle();
    }

    handler->link(handle); // add handle
    new (handler->ptr) T(std::forward<Args>(in)...); // placement new
}

template<typename T> Ptr<T>::Ptr(Ptr&&): handler(in.handler), handle(in.handle) {
    in.handler = nullptr;
    in.handle = nullptr;
}
 

template<typename T> Ptr<T>::Ptr(const Ptr& in) {
    // new handle
    handle = static_cast<Handle*>(std::malloc(sizeof(Handle)));

    // check
    if (handle == nullptr) {
        throw std::bad_alloc(); // failed
    }
    else new (handle) Handle(); // init

    handler = in.handler;  // shared
    handler->link(handle); // add handle
}

template<typename T> Ptr<T>::~Ptr() {
    if (handler) {
        handler->unlink(handle); // pop

        // null == empty
        if (handler->head == nullptr) {
            handler->data.~T(); // call deconstructor
            free(handler);      // Handler destructor not required
        }
    }

    // check for safe
    if (handle) {
        free(handle); // Handle destructor not required
    }
}

template<typename T> bool Ptr<T>::clone() {
    // unique
    if (handler->unique() == true) {
        return;
    }

    //  new for deep copy
    Handler* newly = static_cast<Handler*>(malloc(sizeof(Handler))); 
    if(!newly) {
        return false;
    }

    new (newly) Handler();             // init
    new (newly->ptr) T(handler->data); // copy uses placement new

    // move
    handler->unlink(handle); // pop
    handler = newly;         // new
    handler->link(handle);   // set
}


template<typename T> bool Ptr<T>::Handler::unique() const {
    return head == nullptr || head->next == nullptr;
}

template<typename T> void Ptr<T>::Handler::link(Handle* in) {
    in->prev = nullptr; // [null]<--[ in ]-->[????]
    in->next = head;    // [null]<--[ in ]-->[head]
    if (head) head->prev = in;      // [null]<->[ in ]<->[head]
    head = in;      // [null]<->[head]<->[....]
}

template<typename T> void Ptr<T>::Handler::unlink(Handle* in) {
    if (in == head) {      // [head]<->[....]<->[....]
        head = head->next; // [ in ]<->[head]<->[....]
    }

    if (in->prev) in->prev->next = in->next; // [prev]<--[ in ]-->[next] 
    if (in->next) in->next->prev = in->prev; //    ^----------------^

    in->prev = nullptr; // [prev]   [ in ]-->[next]
    in->next = nullptr; // [prev]<---------->[next]
}

template<typename T> T* Ptr<T>::operator->() {
    return &handler->data;
}

template<typename T> const T* Ptr<T>::operator->() const {
    return &handler->data;
}

template<typename T> T& Ptr<T>::operator*() {
    return handler->data;
}

template<typename T> const T& Ptr<T>::operator*() const {
    return handler->data;
}

}
LWE_END
#endif