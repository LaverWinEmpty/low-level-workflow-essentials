LWE_BEGIN
namespace meta {

// ctor: default
template<typename T> RC<T>::RC():
    ptr(static_cast<Object*>(Object::constructor<T>()),                   // custom allocator
        [](Object* in) { Object::destructor(static_cast<Object*>(in)); }) // custom deallocator
{ }

// copy: shallow
template<typename T> RC<T>::RC(const RC& in): ptr(in.ptr) { }

template<typename T>
template<typename U>
RC<T>::RC(const RC<U>& in) {
    // check failed
    if(!in->isof<T>()) {
        throw diag::error(diag::Code::TYPE_MISMATCH);
    }
    // shallow copy
    ptr = in.ptr;
}

template<typename T> auto RC<T>::operator=(const RC<T>& in) -> RC& {
    ptr = in.ptr;
    return *this;
}

template<typename T> auto RC<T>::operator=(RC<T>&& in) noexcept -> RC& {
    if(this != &in) {
        ptr = std::move(in.ptr);
    }
    return *this;
}

template<typename T> template<typename U> auto RC<T>::operator=(const RC<U>& in) -> RC& {
    if(!in->isof<T>()) {
        throw diag::error(diag::Code::TYPE_MISMATCH);
    }
    ptr = in.ptr;
    return *this;
}

template<typename T> template<typename U> auto RC<T>::operator=(RC<U>&& in) -> RC& {
    if(this != &in) {
        if(!in->isof<T>()) {
            throw diag::error(diag::Code::TYPE_MISMATCH);
        }
        ptr = std::move(in.ptr);
    }
    return *this;
}

template<typename T> RC<T>::RC(RC&& in) noexcept: ptr(std::move(in.ptr)) { }

template<typename T> template<typename U> RC<T>::RC(RC<U>&& in) {
    if(!in->isof<T>()) {
        throw diag::error(diag::Code::TYPE_MISMATCH);
    }
    ptr = std::move(in.ptr);
}

template<typename T> RC<T>::RC(std::nullptr_t ptr): ptr(nullptr) { }

template<typename T> RC<T>::~RC() { }

template<typename T> template<typename U> U* RC<T>::cast() {
    if(!ptr) {
        return nullptr;
    }

    // checked
    if(ptr->isof<U>()) {
        return ptr.as<U>();
    }
    return nullptr;
}

template<typename T> template<typename U> U* RC<T>::as() {
    if(!ptr) {
        return nullptr;
    }
    return ptr.as<U>();
}

template<typename T>
bool RC<T>::clone() {
    return ptr.clone();
}

template<typename T> T* RC<T>::get() {
    if(!ptr) {
        return nullptr;
    }
    return ptr.as<T>();
}

template<typename T> const T* RC<T>::get() const {
    return const_cast<RC<T>*>(this)->get();
}

template<typename T> bool RC<T>::valid() const {
    return ptr.valid();
}

template<typename T> bool RC<T>::owned() const {
    return ptr.owned();
}

template<typename T> bool RC<T>::own() {
    return ptr.own();
}

template<typename T> RC<T>::operator bool() const{
    return valid();
}

} // namespace meta
LWE_END
