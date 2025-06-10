#ifdef LWE_META_REF

LWE_BEGIN
namespace meta {

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

template<typename T> auto Ref<T>::operator=(const Ref<T>& in) -> Ref& {
    ptr = in.ptr;
    return *this;
}

template<typename T> auto Ref<T>::operator=(Ref<T>&& in) noexcept -> Ref& {
    if(this != &in) {
        ptr = std::move(in.ptr);
    }
    return *this;
}

template<typename T> template<typename U> auto Ref<T>::operator=(const Ref<U>& in) -> Ref& {
    if(!in->isof<T>()) {
        throw diag::error(diag::Code::TYPE_MISMATCH);
    }
    ptr = in.ptr;
    return *this;
}

template<typename T> template<typename U> auto Ref<T>::operator=(Ref<U>&& in) -> Ref& {
    if(this != &in) {
        if(!in->isof<T>()) {
            throw diag::error(diag::Code::TYPE_MISMATCH);
        }
        ptr = std::move(in.ptr);
    }
    return *this;
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
template<typename U> diag::Expected<Ref<U>> Ref<T>::cast() {
    // checked
    if (ptr->isof<U>()) {
        return Ref<U>(*this);
    }
    else return diag::error(diag::Code::TYPE_MISMATCH); // cast failed
}

template<typename T>
bool Ref<T>::clone() {
    return ptr.clone();
}

template<typename T> T& Ref<T>::as() {
    return *ptr;
}

template<typename T> const T& Ref<T>::as() const {
    return *ptr;
}

template<typename T> Ref<T>::operator T*() {
    return ptr;
}

}
LWE_END
#endif