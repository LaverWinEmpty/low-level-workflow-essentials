LWE_BEGIN
namespace container {

/**************************************************************************************************
 * Iterator Specialization
 **************************************************************************************************/

template<typename T, size_t SVO> class Iterator<FWD, Stack<T, SVO>> {
    ITERATOR_BODY(FWD, Stack, T, SVO);
public:
    Iterator(T* in) noexcept: ptr(in) { }
    Iterator(const Reverse& in) noexcept: ptr(in.it.ptr) { }
    Iterator& operator=(const Reverse& in) noexcept { return ptr = in.it.ptr, *this; }
    Iterator& operator++() noexcept { return ++ptr, *this; }
    Iterator& operator--() noexcept { return --ptr, *this; }
    Iterator  operator++(int) noexcept { return Iterator(ptr++); }
    Iterator  operator--(int) noexcept { return Iterator(ptr--); }
    bool      operator==(const Iterator& in) const noexcept { return ptr == in.ptr; }
    bool      operator!=(const Iterator& in) const noexcept { return !operator==(in); }
    bool      operator==(const Reverse& in) const noexcept { return *this == in.it; }
    bool      operator!=(const Reverse& in) const noexcept { return *this != in.it; }
    const T&  operator*() const noexcept { return *ptr; }
    const T*  operator->() const noexcept { return ptr; }
    T&        operator*() noexcept { return *ptr; }
    T*        operator->() noexcept { return ptr; }
private:
    T* ptr;
};

// reverse iterator
template<typename T, size_t SVO> class Iterator<BWD, Stack<T, SVO>> {
    ITERATOR_BODY_REVERSE(Stack, T, SVO);
public:
    Iterator(T* in) noexcept: it(in) { }
};

REGISTER_CONST_ITERATOR((typename T, size_t SVO), FWD, Stack, T, SVO);
REGISTER_CONST_ITERATOR((typename T, size_t SVO), BWD, Stack, T, SVO);

/**************************************************************************************************
 * Stack
 **************************************************************************************************/

template<typename T, size_t N>
template<size_t X, bool COPY>
bool Stack<T, N>::ctor(std::conditional_t<COPY, const Stack<T, X>&, Stack<T, X>&&> in, index_t begin) {
    // reset data
    if(counter != 0) {
        clear();
    }
    else if(container == nullptr) {
        container = stack; // init for safe
    }

    if constexpr(COPY == false) {
        // other container is stack -> unable to move
        // counter is less than MIN -> unable to move
        if(in.container != in.stack && in.counter > MIN) {
            if(container != stack) {
                free(container); // free
            }

            container    = in.container; // move
            capacitor    = in.capacitor; // move
            counter      = in.counter;   // move
            in.container = in.stack;     // init
            in.capacitor = in.MIN;       // init
            in.counter   = 0;            // init
            return true;
        }
    }

    // realloc -> auto stack or heap
    else if((capacitor < size_t(in.counter)) && (reallocate(in.counter) == false)) {
        return false; // bad alloc
    }

    // copy or move
    transfer<COPY>(in.container, container, begin, in.counter);

    // copy or move
    counter = in.counter; // set
    if constexpr(!COPY) {
        in.counter = 0; // init
    }
    return true;
}

template<typename T, size_t SVO> Stack<T, SVO>::Stack(): container(stack) { }

template<typename T, size_t SVO> Stack<T, SVO>::~Stack() {
    clear();
    if(container != stack) {
        free(container);
    }
}

template<typename T, size_t SVO> Stack<T, SVO>::Stack(const Stack& in) {
    ctor<SVO, true>(in, 0);
}

template<typename T, size_t SVO> Stack<T, SVO>::Stack(Stack&& in) noexcept {
    ctor<SVO, false>(std::move(in), 0);
}

template<typename T, size_t SVO> auto Stack<T, SVO>::operator=(const Stack& in) -> Stack& {
    if(this != &in) {
        ctor<SVO, true>(in, 0);
    }
    return *this;
}

template<typename T, size_t SVO> auto Stack<T, SVO>::operator=(Stack&& in) noexcept -> Stack& {
    if(this != &in) {
        ctor<SVO, false>(std::move(in), 0);
    }
    return *this;
}

template<typename T, size_t SVO> T& Stack<T, SVO>::operator[](index_t index) noexcept {
    return container[index];
}

template<typename T, size_t SVO> const T& Stack<T, SVO>::operator[](index_t index) const noexcept {
    return container[index];
}

template<typename T, size_t SVO> template<size_t N> Stack<T, SVO>::Stack(const Stack<T, N>& in) {
    ctor<N, true>(in, 0);
}

template<typename T, size_t SVO> template<size_t N> Stack<T, SVO>::Stack(Stack<T, N>&& in) noexcept {
    ctor<N, false>(std::move(in), 0);
}

template<typename T, size_t SVO> template<size_t N> Stack<T, N>& Stack<T, SVO>::operator=(const Stack<T, N>& in) {
    if(this != &in) {
        ctor<N, true>(in, 0);
    }
    return *this;
}

template<typename T, size_t SVO> template<size_t N> Stack<T, N>& Stack<T, SVO>::operator=(Stack<T, N>&& in) noexcept {
    if(this == &in) {
        ctor<N, false>(std::move(in), 0);
    }
    return *this;
}

template<typename T, size_t SVO> template<typename Arg>
bool Stack<T, SVO>::emplace(index_t index, Arg&& in) noexcept {
    // full, bad alloc
    if(counter == capacitor) {
        size_t n = capacitor ? (capacitor << 1) : config::CAPACITY; // set default
        if(!reallocate(n)) {
            return false;
        }
    }

    // construct
    new(container + index) T(std::forward<Arg>(in));

    ++counter;
    return true;
}

template<typename T, size_t SVO> bool Stack<T, SVO>::erase(index_t index, T* out) noexcept {
    if(index < 0 || index >= counter) {
        return false;
    }

    if(out) {
        *out = std::move(container[index]);
    }
    container[index].~T();
    --counter;

    return true;
}

template<typename T, size_t SVO> bool Stack<T, SVO>::push() noexcept {
    return emplace(counter, T{});
}

template<typename T, size_t SVO> bool Stack<T, SVO>::push(T&& in) noexcept {
    return emplace(counter, std::move(in));
}

template<typename T, size_t SVO> bool Stack<T, SVO>::push(const T& in) noexcept {
    return emplace(counter, in);
}

template<typename T, size_t SVO> bool Stack<T, SVO>::pop(T* out) noexcept {
    return erase(counter - 1, out);
}

template<typename T, size_t SVO> bool Stack<T, SVO>::pop(T& out) noexcept {
    return erase(counter - 1, &out);
}

template<typename T, size_t SVO>
template<typename Arg> bool Stack<T, SVO>::insert(index_t index, Arg&& in) {
    if(index < 0) index = 0;
    else if(index >= counter) index = counter;

    // full, bad alloc
    if(counter == capacitor) {
        size_t n = capacitor ? (capacitor << 1) : config::CAPACITY; // set default
        if(!reallocate(n)) {
            return false;
        }
    }

    index_t last = 0;
    if(counter) {
        last = counter - 1;
        // swap and insert
        if(index < last) {
            new(container + last) T(std::move(container[index])); // move
        }
    }

    // construct
    new(container + index) T(std::forward<Arg>(in)); // insert
    ++counter;
    return true;
}

template<typename T, size_t SVO> bool Stack<T, SVO>::remove(index_t index, T* out) {
    if(counter == 0) {
        return false;
    }

    if(index < 0) index = 0;
    else if(index >= counter) index = counter;

    if(out) {
        *out = std::move(container[index]); // return
    }

    // swap and delete
    size_t last = counter - 1;
    if(index < last) {
        container[index] = std::move(container[last]); // move
        container[last].~T();                          // destruct
    }

    else container[index].~T(); // destruct
    --counter;                  // count
}

template<typename T, size_t SVO> bool Stack<T, SVO>::remove(index_t idx, T& out) {
    remove(idx, &out);
}

template<typename T, size_t SVO> bool Stack<T, SVO>::resize(size_t in) noexcept {
    // reallocate
    if(in > capacitor) {
        if(!reallocate(in)) {
            return false;
        }
    }

    // fill
    if(in > counter) {
        for(index_t i = counter; i < in; ++i) {
            new(container + i) T{};
        }
    }
    counter = in; // set

    return true;
}

template<typename T, size_t SVO> bool Stack<T, SVO>::reserve(size_t in) noexcept {
    if(in < capacitor) {
        return true;
    }
    return reallocate(in);
}

template<typename T, size_t SVO> bool Stack<T, SVO>::compact() noexcept {
    return reallocate(counter);
}

template<typename T, size_t SVO> void Stack<T, SVO>::clear() noexcept {
    for(index_t i = 0; i < counter; ++i) {
        container[i].~T();
    }
    counter = 0;
}

template<typename T, size_t SVO> size_t Stack<T, SVO>::size() const noexcept {
    return counter;
}

template<typename T, size_t SVO> size_t Stack<T, SVO>::capacity() const noexcept {
    return capacitor;
}

template<typename T, size_t SVO> inline bool Stack<T, SVO>::full() const noexcept {
    return counter == capacitor;
}

template<typename T, size_t SVO> inline bool Stack<T, SVO>::empty() const noexcept {
    return counter == 0;
}

template<typename T, size_t SVO> T* Stack<T, SVO>::data() noexcept {
    return container;
}

template<typename T, size_t SVO> const T* Stack<T, SVO>::data() const noexcept {
    return const_cast<Stack*>(this)->data();
}

template<typename T, size_t SVO> T& Stack<T, SVO>::at(index_t in) {
    if(in < 0 || in >= counter) {
        throw diag::error(diag::OUT_OF_RANGE);
    }
    return container[in];
}

template<typename T, size_t SVO> const T& Stack<T, SVO>::at(index_t in) const {
    return const_cast<Stack*>(this)->at(in);
}

template<typename T, size_t SVO> auto Stack<T, SVO>::begin() noexcept -> Iterator<FWD> {
    return Iterator<FWD | VIEW>{ container };
}

template<typename T, size_t SVO> auto Stack<T, SVO>::end() noexcept -> Iterator<FWD> {
    return Iterator<FWD | VIEW>{ container + counter }; // overflow safe: bitwise
}

template<typename T, size_t SVO> auto Stack<T, SVO>::rbegin() noexcept -> Iterator<BWD> {
    return Iterator<BWD | VIEW>{ container + counter - 1 }; // overflow safe: bitwise
}

template<typename T, size_t SVO> auto Stack<T, SVO>::rend() noexcept -> Iterator<BWD> {
    return Iterator<BWD | VIEW>{ container - 1 };
}

template<typename T, size_t SVO> T* Stack<T, SVO>::front() noexcept {
    return Iterator<FWD | VIEW>{ begin() };
}

template<typename T, size_t SVO> T* Stack<T, SVO>::rear() noexcept {
    return Iterator<BWD | VIEW>{ rbegin() };
}

template<typename T, size_t SVO> T* Stack<T, SVO>::top() noexcept {
    return Iterator<BWD | VIEW>{ rbegin() };
}

template<typename T, size_t SVO> T* Stack<T, SVO>::bottom() noexcept {
    return Iterator<FWD | VIEW>{ begin() };
}

template<typename T, size_t SVO> auto Stack<T, SVO>::begin() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Stack*>(this)->begin();
}

template<typename T, size_t SVO> auto Stack<T, SVO>::end() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Stack*>(this)->end();
}

template<typename T, size_t SVO> auto Stack<T, SVO>::rbegin() const noexcept -> Iterator<BWD | VIEW> {
    return const_cast<Stack*>(this)->rbegin();
}

template<typename T, size_t SVO> auto Stack<T, SVO>::rend() const noexcept -> Iterator<BWD | VIEW> {
    return const_cast<Stack*>(this)->rend();
}

template<typename T, size_t SVO> const T* Stack<T, SVO>::front() const noexcept {
    return const_cast<Stack*>(this)->front();
}

template<typename T, size_t SVO> const T* Stack<T, SVO>::rear() const noexcept {
    return const_cast<Stack*>(this)->rear();
}

template<typename T, size_t SVO> const T* Stack<T, SVO>::top() const noexcept {
    return const_cast<Stack*>(this)->top();
}

template<typename T, size_t SVO> const T* Stack<T, SVO>::bottom() const noexcept {
    return const_cast<Stack*>(this)->bottom();
}

template<typename T, size_t SVO> template<typename U> void Stack<T, SVO>::push_back(U&& in) {
    push(std::forward<U>(in));
}

template<typename T, size_t SVO> void Stack<T, SVO>::pop_back() {
    pop();
}

template<typename T, size_t SVO> bool Stack<T, SVO>::reallocate(size_t in, index_t begin) {
    // adjust and check
    if(in < MIN) in = MIN; // set MIN
    else if((in = align(in)) > INT64_MAX) {
        in = INT64_MAX; // limit
    }
    if(in == capacitor) return true;

    T* newly = stack; // init
    if(in > MIN) {
        if((newly = static_cast<T*>(malloc(sizeof(T) * in))) == nullptr) {
            return false; // failed
        }
    }

    // move
    if(counter) {
        transfer<false>(container, newly, begin, counter);
    }

    // release
    if(container != stack) {
        std::free(container);
    }
    container = newly;
    capacitor = in;
    return true;
}

template<typename T, size_t SVO>
template<bool COPY> void Stack<T, SVO>::transfer(const T* in, T* out, index_t begin, size_t size) {
    // logic for cirulation ...
    size_t adjust    = size < capacitor ? size : capacitor;    // set for reduce
    size_t length    = size - begin;                           // begin ~ end
    size_t current   = adjust <= length ? adjust : length;     // begin ~ end size
    size_t remainder = adjust <= length ? 0 : adjust - length; // 0 ~ begin size

    for(size_t i = 0; i < current; ++i) {
        if constexpr(!COPY) {
            new(out + i) T{ std::move(in[begin + i]) };
        }
        else new(out + i) T{ in[begin + i] };
    }
    for(size_t i = 0; i < remainder; ++i) {
        if constexpr(!COPY) {
            new(out + current + i) T{ std::move(in[i]) }; // continue
        }
        else new(out + current + i) T{ in[i] }; // continue
    }
}

template<typename T, size_t SVO> void Stack<T, SVO>::clear(size_t begin) {
    if(begin == 0) {
        clear();
        return;
    }

    size_t length    = capacitor - begin;                        // begin ~ end
    size_t current   = counter <= length ? counter : length;     // begin ~ end size
    size_t remainder = counter <= length ? 0 : counter - length; // 0 ~ begin size

    // begin ~ container end
    for(size_t i = 0; i < size; ++i) {
        container[i].~T();
    }
    for(size_t i = 0; i < remainder; ++i) {
        container[i + current].~T();
    }

    counter = 0;
}

} // namespace container
LWE_END
