#ifdef LWE_SYS_EXCEPTIONAL

LWE_BEGIN
namespace diag {

template<typename T> Expected<T>::Expected(const Alert& in) noexcept: flag(false) {
    new(&msg) Alert(in);
}

template<typename T> Expected<T>::Expected(Alert&& in) noexcept: flag(false) {
    new(&msg) Alert(std::move(in));
}

template<typename T> Expected<T>::Expected(const T& in) noexcept: flag(true) {
    new(&data) T(in);
}

template<typename T> Expected<T>::Expected(T&& in) noexcept: flag(true) {
    new(&data) T(std::move(in));
}

template<typename T> Expected<T>::Expected(const Expected& in) noexcept: flag(in.flag) {
    if(flag) {
        new(&data) T(in.data);
    }
    else new(&msg) Alert(in.msg);
}

template<typename T> Expected<T>::Expected(Expected&& in) noexcept: flag(in.flag) {
    if(flag) {
        new(&data) T(std::move(in.data));
    }
    else new(&msg) Alert(std::move(in.msg));
}

template<typename T> Expected<T>::~Expected() {
    if(flag) {
        data.~T();
    }
    else msg.~Alert();
}

template<typename T> auto Expected<T>::operator=(const Expected& in) noexcept -> Expected& {
    if(flag) {
        data.~T();
    }
    else msg.~Alert();

    if(flag = in.flag) {
        new(&data) T(in.data);
    }
    else new(&msg) Alert(in.msg);
    return *this;
}

template<typename T> auto Expected<T>::operator=(Expected&& in) noexcept -> Expected& {
    if(this != &in) {
        if(flag) {
            data.~T();
        }
        else msg.~Alert();

        if(flag = in.flag) {
            new(&data) T(std::move(in.data));
        }
        else new(&msg) Alert(std::move(in.msg));
    }
    return *this;
}

template<typename T> Expected<T>::operator bool() const {
    return flag;
}

template<typename T> Expected<T>::operator const Alert&() const {
    return msg;
}

template<typename T> Expected<T>::operator const T&() const {
    return data;
}

template<typename T> Expected<T>::operator const char*() const {
    return what();
}

template<typename T> T* Expected<T>::operator->() {
    if(!flag) {
        throw msg;
    }
    return &data;
}

template<typename T> T& Expected<T>::operator*() {
    if(!flag) {
        throw msg;
    }
    return data;
}

template<typename T> const T* Expected<T>::operator->() const {
    return const_cast<Expected<T>*>(this)->operator->();
}

template<typename T> const T& Expected<T>::operator*() const {
    return const_cast<Expected<T>*>(this)->operator*();
}

template<typename T> const char* Expected<T>::what() const {
    if(flag) {
        return "";
    }
    return msg.what();
}

template<typename T> T&& Expected<T>::move() {
    if(!flag) {
        throw msg;
    }
    return std::move(data);
}

template<typename T> T& Expected<T>::as() {
    if(!flag) {
        throw msg;
    }
    return data;
}

} // namespace diag
LWE_END
#endif
