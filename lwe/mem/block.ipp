LWE_BEGIN
namespace mem {

// not available
template<size_t N> struct Block<N, void> { };

// size 0 specialize
template<typename T> struct Block<0, T> {
    constexpr T&                            operator[](size_t) { return *reinterpret_cast<T*>(this); }
    constexpr const T&                      operator[](size_t) const { return *reinterpret_cast<const T*>(this); }
    constexpr                               operator T*() noexcept { return nullptr; }
    constexpr                               operator const T*() const noexcept { return nullptr; }
    constexpr T&                            at(size_t) { return diag::error(diag::OUT_OF_RANGE); }
    constexpr T&                            at(size_t) const { return diag::error(diag::OUT_OF_RANGE); }
    constexpr T*                            data() noexcept { return nullptr; }
    constexpr const T*                      data() const noexcept { return nullptr; }
    template<typename U> constexpr U*       as() noexcept { return nullptr; }
    template<typename U> constexpr const U* as() const noexcept { return nullptr; }
    static constexpr size_t                 size() noexcept { return 0; }
    constexpr const T*                      begin() const noexcept { return nullptr; }
    constexpr const T*                      end() const noexcept { return nullptr; }
};

template<size_t N, typename T> constexpr Block<N, T>::Block(const T* in, size_t len) {
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

template<size_t N, typename T> constexpr Block<N, T>::Block(const std::initializer_list<T>& in) {
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
template<typename> constexpr Block<N, T>::Block(const string& in):
    Block(reinterpret_cast<const T*>(in.c_str()), in.size() + 1) {
    stack[N - 1] = 0; // copy with null, safe for size exceeded
}

template<size_t N, typename T>
template<typename> constexpr Block<N, T>::Block(const char* in):
    Block(reinterpret_cast<const T*>(in), std::strlen(in) + 1) {
    stack[N - 1] = 0; // copy with null, safe for size exceeded
}

template<size_t N, typename T> constexpr T& Block<N, T>::operator[](size_t index) noexcept {
    return stack[index];
}

template<size_t N, typename T> constexpr const T& Block<N, T>::operator[](size_t index) const noexcept {
    return stack[index];
}

template<size_t N, typename T> constexpr Block<N, T>::operator const T*() const noexcept {
    return stack;
}

template<size_t N, typename T> constexpr Block<N, T>::operator T*() noexcept {
    return stack;
}

template<size_t N, typename T> constexpr T& Block<N, T>::at(size_t index) {
    if(index >= N) {
        throw diag::error(diag::OUT_OF_RANGE);
    }
    return stack[index];
}

template<size_t N, typename T> constexpr const T& Block<N, T>::at(size_t index) const {
    if(index >= N) {
        throw diag::error(diag::OUT_OF_RANGE);
    }
    return stack[index];
}

template<size_t N, typename T> constexpr T* Block<N, T>::data() noexcept {
    return stack;
}

template<size_t N, typename T> constexpr const T* Block<N, T>::data() const noexcept {
    return stack;
}

template<size_t N, typename T> template<typename U> constexpr U* Block<N, T>::as() noexcept {
    return reinterpret_cast<U*>(stack);
}

template<size_t N, typename T> template<typename U> constexpr const U* Block<N, T>::as() const noexcept {
    return reinterpret_cast<U*>(stack);
}

template<size_t N, typename T> constexpr size_t Block<N, T>::size() noexcept {
    return N;
}

template<size_t N, typename T> constexpr T* Block<N, T>::begin() noexcept {
    return stack;
}

template<size_t N, typename T> constexpr const T* Block<N, T>::begin() const noexcept {
    return stack;
}

template<size_t N, typename T> constexpr T* Block<N, T>::end() noexcept {
    return stack + N - 1;
}

template<size_t N, typename T> constexpr const T* Block<N, T>::end() const noexcept {
    return stack + N - 1;
}
} // namespace mem
LWE_END
