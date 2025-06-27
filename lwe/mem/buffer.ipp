LWE_BEGIN
namespace mem {

//  void specialize -> dummy block (not available)
template<size_t N> struct Buffer<N, void> {
private:
    template<size_t, typename> friend struct Buffer;
    static int8_t unused[N];
};
template<size_t N> int8_t Buffer<N, void>::unused[N]; // for dangling

// empty blcok specialize
template<> struct Buffer<0, void> {
    static int8_t unused[1];
};
int8_t Buffer<0, void>::unused[1]; // for dangling

// size 0 specialize
template<typename T> struct Buffer<0, T> {
    constexpr T&                            operator[](size_t) noexcept { return *unused; }
    constexpr const T&                      operator[](size_t) const noexcept { return *unused; }
    constexpr                               operator T*() noexcept { return nullptr; }
    constexpr                               operator const T*() const noexcept { return nullptr; }
    constexpr T&                            at(size_t) noexcept { return diag::error(diag::OUT_OF_RANGE); }
    constexpr T&                            at(size_t) const noexcept { return diag::error(diag::OUT_OF_RANGE); }
    constexpr T*                            data() noexcept { return nullptr; }
    constexpr const T*                      data() const noexcept { return nullptr; }
    template<typename U> constexpr U*       as() noexcept { return nullptr; }
    template<typename U> constexpr const U* as() const noexcept { return nullptr; }
    static constexpr size_t                 size() noexcept { return 0; }
    constexpr const T*                      begin() const noexcept { return nullptr; }
    constexpr const T*                      end() const noexcept { return nullptr; }
private:
    static T* unused; //!< space for dangling pointer defense. should not be used.
};
template<typename T> T* Buffer<0, T>::unused = reinterpret_cast<T*>(Buffer<sizeof(T), void>::unused);

template<size_t N, typename T> constexpr Buffer<N, T>::Buffer(const T* in, size_t len) {
    size_t n = N < len ? N : len;
    if constexpr(std::is_trivially_copyable_v<T>) {
        std::memcpy(stack, in, sizeof(T) * n);
    }
    else {
        for(int i = 0; i < n; ++i) {
            stack[i] = in[i];
        }
    }
}

template<size_t N, typename T> constexpr Buffer<N, T>::Buffer(const std::initializer_list<T>& in) {
    size_t loop = in.size();
    if(loop >= N) {
        loop = N;
    }
    auto itr = in.begin();
    for(int i = 0; i < loop; ++i) {
        stack[i] = *itr;
        ++itr;
    }
}

template<size_t N, typename T>
template<typename> constexpr Buffer<N, T>::Buffer(const string& in):
    Buffer(reinterpret_cast<const T*>(in.c_str()), in.size() + 1) {
    stack[N - 1] = 0; // copy with null, safe for size exceeded
}

template<size_t N, typename T>
template<typename> constexpr Buffer<N, T>::Buffer(const char* in):
    Buffer(reinterpret_cast<const T*>(in), std::strlen(in) + 1) {
    stack[N - 1] = 0; // copy with null, safe for size exceeded
}

template<size_t N, typename T> constexpr T& Buffer<N, T>::operator[](size_t index) noexcept {
    return stack[index];
}

template<size_t N, typename T> constexpr const T& Buffer<N, T>::operator[](size_t index) const noexcept {
    return stack[index];
}

template<size_t N, typename T> constexpr Buffer<N, T>::operator const T*() const noexcept {
    return stack;
}

template<size_t N, typename T> constexpr Buffer<N, T>::operator T*() noexcept {
    return stack;
}

template<size_t N, typename T> constexpr diag::Expected<T&> Buffer<N, T>::at(size_t index) noexcept {
    if(index >= N) return diag::error(diag::OUT_OF_RANGE);
    return stack[index];
}

template<size_t N, typename T> constexpr diag::Expected<const T&> Buffer<N, T>::at(size_t index) const noexcept {
    if(index >= N) return diag::error(diag::OUT_OF_RANGE);
    return stack[index];
}

template<size_t N, typename T> constexpr T* Buffer<N, T>::data() noexcept {
    return stack;
}

template<size_t N, typename T> constexpr const T* Buffer<N, T>::data() const noexcept {
    return stack;
}

template<size_t N, typename T> template<typename U> constexpr U* Buffer<N, T>::as() noexcept {
    return reinterpret_cast<U*>(stack);
}

template<size_t N, typename T> template<typename U> constexpr const U* Buffer<N, T>::as() const noexcept {
    return reinterpret_cast<U*>(stack);
}

template<size_t N, typename T> constexpr size_t Buffer<N, T>::size() noexcept {
    return N;
}

template<size_t N, typename T> constexpr T* Buffer<N, T>::begin() noexcept {
    return stack;
}

template<size_t N, typename T> constexpr const T* Buffer<N, T>::begin() const noexcept {
    return stack;
}

template<size_t N, typename T> constexpr T* Buffer<N, T>::end() noexcept {
    return stack + N - 1;
}

template<size_t N, typename T> constexpr const T* Buffer<N, T>::end() const noexcept {
    return stack + N - 1;
}
} // namespace mem
LWE_END
