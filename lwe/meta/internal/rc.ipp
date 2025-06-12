#ifdef LWE_META_RC

LWE_BEGIN
namespace meta {

// ctor: default
template<typename T> RC<T>::RC():
    ptr(
        static_cast<Object*>(Object::constructor<T>()),
        // custom deallocator
        [](void* in) {
            Object::destructor(static_cast<Object*>(in));
        } // end lambda
    ) // end ptr()
{}

// copy: shallow
template<typename T> RC<T>::RC(const RC& in): ptr(in.ptr) {}

template<typename T>
template<typename U>
RC<T>::RC(const RC<U>& in) {
    // check failed
    if (!in->isof<T>()) {
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

template<typename T> RC<T>::RC(RC&& in) noexcept: ptr(std::move(in.ptr)) {}

template<typename T> template<typename U> RC<T>::RC(RC<U>&& in) {
    if(!in->isof<T>()) {
        throw diag::error(diag::Code::TYPE_MISMATCH);
    }

    ptr = std::move(in.ptr);
}

template<typename T> RC<T>::~RC() {}

template<typename T> template<typename U> U* RC<T>::cast() {
    // checked
    if (ptr->isof<U>()) {
        return RC<U>(*this);
    }
    return nullptr;
}

template<typename T> template<typename U> U* RC<T>::as() {
    return reinterpret_cast<U*>(ptr);
}

template<typename T>
bool RC<T>::clone() {
    return ptr.clone();
}

template<typename T> T& RC<T>::as() {
    return *ptr;
}

template<typename T> const T& RC<T>::as() const {
    return *ptr;
}

template<typename T> RC<T>::operator T*() {
    return ptr;
}

}
LWE_END
#endif