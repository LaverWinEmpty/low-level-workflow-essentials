LWE_BEGIN
namespace meta {

template<typename T> Reflector<T>::Reflector(const Reflector& in): capacitor(in.capacitor), count(in.count) {
    // set
    capacitor = in.capacitor;
    count     = in.count;
    if(count == 0) {
        data = nullptr;
        return;
    }

    // new and copy
    data = static_cast<T*>(malloc(sizeof(T) * capacitor));
    if(!data) {
        capacitor = 0;
        count     = 0;
        throw std::bad_alloc();
    }
    std::memcpy(data, in.data, sizeof(T) * capacitor);
}

template<typename T>
Reflector<T>::Reflector(Reflector&& in) noexcept: data(in.data), count(in.count), capacitor(in.capacitor) {
    in.data      = nullptr;
    in.capacitor = 0;
    in.count     = 0;
}

template<typename T> Reflector<T>::Reflector(const std::initializer_list<T>& in): count(0), capacitor(in.size()) {
    data = static_cast<Field*>(malloc(sizeof(Field) * capacitor));
    if(!data) {
        throw std::bad_alloc();
    }
    for(auto& i : in) {
        data[count++] = i;
    }
}

template<typename T> Reflector<T>::~Reflector() {
    if(data) {
        free(data);
    }
}

template<typename T> Reflector<T>& Reflector<T>::operator=(const Reflector in) {
    if(data) {
        free(data);
    }

    // set
    count     = in.count;
    capacitor = in.capacitor;
    if(count == 0) {
        data = nullptr;
        return *this;
    }

    // new and copy
    data = static_cast<Field*>(malloc(sizeof(Field) * capacitor));
    if(!data) {
        count     = 0;
        capacitor = 0;
        throw std::bad_alloc();
    }
    std::memcpy(data, in.data, sizeof(Field) * capacitor);

    return *this;
}

template<typename T> Reflector<T>& Reflector<T>::operator=(Reflector&& in) noexcept {
    if(this != &in) {
        if(data) {
            free(data);
        }
        data         = in.data;
        count        = in.count;
        capacitor    = in.capacitor;
        in.data      = nullptr;
        in.capacitor = 0;
        in.count     = 0;
    }
    return *this;
}

template<typename T> const T& Reflector<T>::operator[](size_t in) const {
    return data[in];
}

template<typename T> const T* Reflector<T>::begin() const {
    return data;
}

template<typename T> const T* Reflector<T>::end() const {
    return data + count;
}

template<typename T> size_t Reflector<T>::size() const {
    return count;
}

template<typename T> void Reflector<T>::shrink() {
    T* newly = static_cast<T*>(realloc(data, sizeof(T) * count));
    if(!newly) {
        throw std::bad_alloc();
    }
    data      = newly;
    capacitor = count;
}

template<typename T> template<class C> const Reflector<T>& Reflector<T>::reflect() {
    // default
    static Reflector<T> empty;
    return empty;
}

template<typename T> template<typename C> const Reflector<T>& Reflector<T>::find() {
    static Reflector statics = reflect<C>();
    return statics;
}

template<typename T> template<typename C> const Reflector<T>& Reflector<T>::find(const C&) {
    return find<C>();
}

template<typename T> const Reflector<T>& Reflector<T>::find(const String& in) {
    return map()[in];
}

template<typename T> const Reflector<T>& Reflector<T>::find(const char* in) {
    return find(String{ in });
}

template<typename T> template<typename Arg> void Reflector<T>::push(Arg&& in) {
    if(count >= capacitor) {
        capacitor += 256;
        T* newly   = static_cast<T*>(realloc(data, sizeof(T) * capacitor));
        if(!newly) {
            capacitor >>= 1;
            throw std::bad_alloc();
        }
        data = newly;
    }
    new(data + count) T(std::forward<Arg>(in));
    ++count;
}

template<typename T> auto Reflector<T>::map() -> Table& {
    static Table instance;
    return instance;
}

} // namespace meta
LWE_END
