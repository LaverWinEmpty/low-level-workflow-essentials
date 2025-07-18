LWE_BEGIN
namespace container {

template<typename T, size_t SVO> class Iterator<FWD, Deque<T, SVO>> {
    ITERATOR_BODY(FWD, Deque, T, SVO);
public:
    Iterator(T* container, index_t index, size_t capacity) noexcept: ptr(container), idx(index), cap(capacity) { }
    Iterator(const Reverse& in) noexcept: Iterator(in.it) { }
    Iterator& operator++() noexcept { return ++idx, *this; }
    Iterator& operator--() noexcept { return --idx, *this; }
    Iterator  operator++(int) noexcept { return Iterator(ptr, idx++, cap); }
    Iterator  operator--(int) noexcept { return Iterator(ptr, idx--, cap); }
    bool      operator==(const Iterator& in) const noexcept { return ptr == in.ptr && idx == in.idx; }
    bool      operator!=(const Iterator& in) const noexcept { return !operator==(in); }
    bool      operator==(const Reverse& in) const noexcept { return *this == in.it; }
    bool      operator!=(const Reverse& in) const noexcept { return *this != in.it; }
    const T&  operator*() const noexcept { return *(ptr + (idx & (cap - 1))); }
    const T*  operator->() const noexcept { return (ptr + (idx & (cap - 1))); }
    T&        operator*() noexcept { return *(ptr + (idx & (cap - 1))); }
    T*        operator->() noexcept { return (ptr + (idx & (cap - 1))); }

private:
    T*      ptr;
    index_t idx;
    size_t  cap;
};

template<typename T, size_t SVO> class Iterator<BWD, Deque<T, SVO>> {
    ITERATOR_BODY_REVERSE(Deque, T, SVO);
public:
    Iterator(T* container, index_t index, size_t capacity) noexcept: it(container, index, capacity) { }
};

template<typename T, size_t SVO> index_t Deque<T, SVO>::absidx(index_t in) const noexcept {
    return (in) & (stack.capacitor - 1);
}

template<typename T, size_t SVO> index_t Deque<T, SVO>::relidx(index_t in) const noexcept {
    return (head + in) & (stack.capacitor - 1);
}

template<typename T, size_t SVO> Deque<T, SVO>::Deque(const Deque& in) {
    stack.ctor<SVO, true>(in.stack, in.head);
}

template<typename T, size_t SVO> Deque<T, SVO>::Deque(Deque&& in) noexcept {
    stack.ctor<SVO, false>(std::move(in.stack), in.head);
    in.clear();
}

template<typename T, size_t SVO> auto Deque<T, SVO>::operator=(const Deque& in) -> Deque& {
    if(this != &in) {
        stack.ctor<SVO, true>(in.stack, in.head);
    }
    return *this;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::operator=(Deque&& in) noexcept -> Deque& {
    if(this != &in) {
        stack.ctor<SVO, false>(std::move(in.stack), in.head);
        in.clear();
    }
    return *this;
}

template<typename T, size_t SVO>
template<size_t X> Deque<T, SVO>::Deque(const Deque<T, X>& in) {
    stack.ctor<SVO, true>(in.stack, in.head);
}

template<typename T, size_t SVO>
template<size_t X> Deque<T, SVO>::Deque(Deque<T, X>&& in) noexcept {
    stack.ctor<SVO, false>(std::move(in.stack), in.head);
    in.clear();
}

template<typename T, size_t SVO>
template<size_t X> auto Deque<T, SVO>::operator=(const Deque<T, X>& in) -> Deque& {
    if(this != &in) {
        stack.ctor<SVO, true>(in.stack, in.head);
    }
    return *this;
}

template<typename T, size_t SVO>
template<size_t X> auto Deque<T, SVO>::operator=(Deque<T, X>&& in) noexcept -> Deque& {
    if(this != &in) {
        stack.ctor<SVO, false>(std::move(in.stack), in.head);
        in.clear();
    }
    return *this;
}

template<typename T, size_t SVO> T& Deque<T, SVO>::operator[](size_t idx) {
    idx = relidx(idx);
    return stack.container[idx];
}

template<typename T, size_t SVO> const T& Deque<T, SVO>::operator[](size_t idx) const {
    return const_cast<Deque*>(this)->operator[](idx);
}

template<typename T, size_t SVO>
bool Deque<T, SVO>::push(const T& in) {
    if(!emplace(tail, in)) {
        return false;
    }
    tail = absidx(tail + 1); // after (tail + 1) % cap
    return true;
}

template<typename T, size_t SVO>
bool Deque<T, SVO>::push(T&& in) {
    if(!emplace(tail, std::move(in))) {
        return false;
    }
    tail = absidx(tail + 1); // after (tail + 1) % cap
    return true;
}

template<typename T, size_t SVO>
bool Deque<T, SVO>::pop(T* out) {
    index_t prev = absidx(tail - 1); // befre
    if(!erase(prev, out)) {
        return false;
    }
    tail = prev;
    return true;
}

template<typename T, size_t SVO>
bool Deque<T, SVO>::pop(T& out) {
    return pop(&out);
}

template<typename T, size_t SVO>
bool Deque<T, SVO>::prepend(const T& in) {
    index_t next = absidx(head - 1); // before (head - 1) % cap
    if(!emplace(next, in)) {
        return false;
    }
    head = next;
    return true;
}

template<typename T, size_t SVO>
bool Deque<T, SVO>::prepend(T&& in) {
    index_t next = absidx(head - 1); // before (head - 1) % cap
    if(!emplace(next, std::move(in))) {
        return false;
    }
    head = next;
    return true;
}

template<typename T, size_t SVO>
bool Deque<T, SVO>::pull(T* out) {
    if(!erase(head, out)) {
        return false;
    }
    head = absidx(head + 1); // after (head + 1) % cap
    return true;
}

template<typename T, size_t SVO>
bool Deque<T, SVO>::pull(T& out) {
    return pull(&out);
}

template<typename T, size_t SVO>
template<typename Arg> bool Deque<T, SVO>::insert(index_t index, Arg&& in) {
    // adjust
    if(index < 0) index = -1;
    else if(index > stack.counter) index = stack.counter;

    // full -> allocate (for swap)
    if(stack.counter == stack.capacitor) {
        if(!stack.allocate(stack.capacitor << 1, head)) {
            return false; // bad alloc
        }
    }

    // get index (head + index) % cap
    index_t rel = relidx(index);

    // swap
    if(index >= 0 && index < stack.counter) {
        new(stack.container + tail) T(std::move(stack.container[rel])); // move
        stack.container[rel].~T();                                      // destruct
    }

    // and insert
    emplace(rel, std::forward<Arg>(in));
    return true;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::remove(index_t index, T* out) {
    if(stack.counter == 0) {
        return false;
    }

    // adjust
    if(index < 0) index = -1;
    else if(index > stack.counter) index = stack.counter;

    // get index (head + index) % cap
    index_t rel = relidx(index);

    // out
    if(out) *out = std::move(stack.container[rel]);

    if(index >= 0 && index < stack.counter) {
        // swap and delete
        if(index != stack.counter - 1) {
            stack.container[rel] = std::move(stack.container[tail]); // move
        }
        stack.container[tail].~T(); // destruct
        tail = absidx(tail - 1);    // at tail
    }
    // fisrt
    else {
        stack.container[head].~T(); // destruct
        head = absidx(head + 1);    // at head
    }
    --stack.counter;
    return true;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::remove(index_t index, T& out) {
    remove(index, &out);
}

template<typename T, size_t SVO> bool Deque<T, SVO>::resize(size_t in) noexcept {
    // reallocate
    if(in > stack.capacitor) {
        if(!reallocate(in)) {
            return false;
        }
    }

    // fill
    if(in > stack.counter) {
        for(index_t i = stack.counter; i < in; ++i) {
            new(stack.container + tail) T{}; // add
            tail = absidx(tail + 1);         // next
        }
    }
    stack.counter = in; // set

    return true;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::reserve(size_t in) noexcept {
    if(in < stack.capacitor) {
        return true;
    }
    return reallocate(in);
}

template<typename T, size_t SVO> bool Deque<T, SVO>::compact() noexcept {
    return reallocate(stack.counter);
}

template<typename T, size_t SVO> void Deque<T, SVO>::clear() noexcept {
    for(index_t i = 0; i < stack.counter; ++i) {
        stack.container[head].~T(); // delete
        head = absidx(head + 1);    // move
    }
    stack.counter = 0; // init
    head          = 0; // init
    tail          = 0; // init
}

template<typename T, size_t SVO> size_t Deque<T, SVO>::size() const noexcept {
    return stack.counter;
}

template<typename T, size_t SVO> size_t Deque<T, SVO>::capacity() const noexcept {
    return stack.capacitor;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::full() const noexcept {
    return stack.counter == stack.capacitor;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::empty() const noexcept {
    return stack.counter == 0;
}


template<typename T, size_t SVO> T* Deque<T, SVO>::data() noexcept {
    return stack.container;
}

template<typename T, size_t SVO> const T* Deque<T, SVO>::data() const noexcept {
    return const_cast<Deque*>(this)->data();
}

template<typename T, size_t SVO> T& Deque<T, SVO>::at(index_t in) {
    if(in < 0 || in >= stack.counter) {
        throw diag::error(diag::OUT_OF_RANGE);
    }
    return stack.container[relidx(in)];
}

template<typename T, size_t SVO> const T& Deque<T, SVO>::at(index_t in) const {
    return const_cast<Deque*>(this)->at(in);
}

template<typename T, size_t SVO> auto Deque<T, SVO>::begin() noexcept -> Iterator<FWD> {
    return Iterator<FWD>(stack.container, head, stack.capacitor);
}

template<typename T, size_t SVO> auto Deque<T, SVO>::end() noexcept -> Iterator<FWD> {
    index_t last = tail;
    if(tail <= head && stack.counter) {
        last += stack.capacitor; // circulation -> unfold
    }
    return Iterator<FWD>(stack.container, last, stack.capacitor);
}

template<typename T, size_t SVO> auto Deque<T, SVO>::rbegin() noexcept -> Iterator<BWD> {
    index_t last = tail - 1;
    if(tail <= head && stack.counter) {
        last += (stack.capacitor); // circulation -> unfold
    }
    return Iterator<BWD>(stack.container, last, stack.capacitor);
}

template<typename T, size_t SVO> auto Deque<T, SVO>::rend() noexcept -> Iterator<BWD> {
    return Iterator<BWD>(stack.container, head - 1, stack.capacitor);
}

template<typename T, size_t SVO>
template<typename Arg> bool Deque<T, SVO>::emplace(index_t index, Arg&& in) {
    // full -> reallocate
    if(stack.counter == stack.capacitor) {
        size_t newcap = stack.capacitor << 1;

        index_t rel = index - head; // abs to rel (inversion -> (head + index) % cap)
        if(rel == 0) {
            rel = stack.capacitor; // adjust wrap-around (on full)
        }
        index = rel & (newcap - 1);

        // head to 0
        if(!reallocate(newcap)) {
            return false;
        }
    }

    // direct handling (ignore reallocate and swap)
    new(stack.container + index) T(std::forward<Arg>(in)); // construct
    ++stack.counter;                                       // add
    return true;
}

template<typename T, size_t SVO>
bool Deque<T, SVO>::erase(index_t index, T* out) {
    if(stack.counter == 0) {
        return false; // failed
    }

    T* data = stack.container + index; // get

    // range not check and delete
    if(out) {
        *out = std::move(*data);
    }
    data->~T(); // destruct
    --stack.counter;
    return true;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::reallocate(size_t size) {
    if(!stack.reallocate(size, head)) {
        return false;
    }
    head = 0;
    tail = stack.counter;
    return true;
}

} // namespace container
LWE_END
