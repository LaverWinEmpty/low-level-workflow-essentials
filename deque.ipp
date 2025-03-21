#ifdef LWE_CONTAINER_DEQUE_HEADER
LWE_BEGIN

namespace stl {

template<typename T, size_t SVO> Deque<T, SVO>::Deque(): Container() {}

template<typename T, size_t SVO> Deque<T, SVO>::~Deque() {
    clear();
    if(container != stack) {
        free(container);
    }
}

template<typename T, size_t SVO> T& Deque<T, SVO>::operator[](index_t index) const noexcept {
    return container[relative(index)];
}

template<typename T, size_t SVO> template<size_t N> Deque<T, SVO>::Deque(const Deque<T, N>& in) {
    // allocate: auto container set heap or stack / and auto set capacitor
    if(!reallocate(in.counter)) {
        throw std::bad_alloc(); // failed
    }

    // copy
    for(size_t i = 0; i < in.counter; ++i) {
        container[i] = in.container[relative(i)];
    }

    // init
    counter = in.counter;
    head    = 0;
    tail    = counter - 1;
}

template<typename T, size_t SVO> template<size_t N> Deque<T, SVO>::Deque(Deque<T, N>&& in) {
    // use stack: unable to move -> allocate
    if(in.container == in.stack) {
        if(!reallocate(in.counter)) {
            throw std::bad_alloc(); // failed
        }

        // move
        for(index_t i = 0; i < in.counter; ++i) {
            new(container + i) T(std::move(in.container[relative(i)]));
        }

        counter = in.counter;
        head    = 0;
        tail    = counter - 1;
    }

    // move pointer
    else {
        container = in.container;
        capacitor = in.capacitor;
        head      = in.head;
        tail      = in.tail;
    }

    // set other
    in.container = in.stack;
    in.capacitor = in.MIN;
    in.counter   = 0;
    in.head      = 0;
    in.tail      = -1;
}

template<typename T, size_t SVO> template<size_t N> Deque<T, N>& Deque<T, SVO>::operator=(const Deque<T, N>& in) {
    clear();

    if(!reallocate(in.counter)) {
        throw std::bad_alloc();
    }

    for(size_t i = 0; i < in.counter; ++i) {
        container[i] = in.container[relative(i)];
    }

    return *this;
}

template<typename T, size_t SVO> template<size_t N> Deque<T, N>& Deque<T, SVO>::operator=(Deque<T, N>&& in) {
    if(this != &in) {
        // counter == stack == not allocated
        if(in.container == in.stack) {
            if(!reallocate(in.counter)) {
                throw std::bad_alloc();
            }

            for(index_t i = 0; i < in.counter; ++i) {
                new(container + i) T(std::move(in.container[i]));
            }

            counter = in.counter;
            head    = 0;
            tail    = counter - 1;
        }

        else {
            container = in.container;
            capacitor = in.capacitor;
            head      = in.head;
            tail      = in.tail;
        }

        // set other
        in.container = in.stack;
        in.capacitor = in.MIN;
        in.counter   = 0;
        in.head      = 0;
        in.tail      = -1;
    }
    return *this;
}

template<typename T, size_t SVO> template<typename Arg> bool Deque<T, SVO>::emplace(index_t index, Arg&& in) noexcept {
    if((counter == capacitor && !reserve(capacitor << 1))) {
        return false;
    }

    // init
    if(counter == 0) {
        index = 0;
        head  = 0;
        tail  = 0;
    }
    // push front
    else if(index <= 0) {
        head  = forward(head);
        index = head;
    }
    // push back
    else if(index >= counter) {
        tail  = backward(tail);
        index = tail;
    }
    // in range: move
    else {
        index = relative(index);
        for(index_t i = tail; i >= index; --i) {
            container[relative(i + 1)] = std::move(container[relative(i)]);
        }
    }

    // construct
    new(container + index) T(std::forward<Arg>(in));

    ++counter;
    return true;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::erase(index_t index, T* out) noexcept {
    if(index < 0 || index >= counter) {
        return false;
    }

    if(out) {
        *out = std::move(container[relative(index)]);
    } else container[relative(index)].~T();
    --counter;

    // pop front -> move cursor only
    if(index == 0) {
        head = backward(head);
        return true;
    }

    // index == last: pop back -> move cursor only
    for(index_t i = index; i < counter; ++i) {
        container[relative(i)] = std::move(container[relative(i + 1)]);
    }
    tail = forward(tail);
    return true;
}

template<typename T, size_t SVO> inline bool Deque<T, SVO>::push() noexcept {
    return emplace(counter, T{});
}

template<typename T, size_t SVO> inline bool Deque<T, SVO>::unshift() noexcept {
    return emplace(-1, T{});
}

template<typename T, size_t SVO> bool Deque<T, SVO>::push(T&& in) noexcept {
    return emplace(counter, std::move(in));
}

template<typename T, size_t SVO> bool Deque<T, SVO>::push(const T& in) noexcept {
    return emplace(counter, in);
}

template<typename T, size_t SVO> bool Deque<T, SVO>::unshift(T&& in) noexcept {
    return emplace(-1, std::move(in));
}

template<typename T, size_t SVO> bool Deque<T, SVO>::unshift(const T& in) noexcept {
    return emplace(-1, in);
}

template<typename T, size_t SVO> bool Deque<T, SVO>::pop(T* out) noexcept {
    return erase(counter - 1, out);
}

template<typename T, size_t SVO> bool Deque<T, SVO>::pop(T& out) noexcept {
    return erase(counter - 1, &out);
}

template<typename T, size_t SVO> bool Deque<T, SVO>::shift(T* out) noexcept {
    return erase(0, out);
}

template<typename T, size_t SVO> bool Deque<T, SVO>::shift(T& out) noexcept {
    return erase(0, &out);
}

template<typename T, size_t SVO> bool Deque<T, SVO>::resize(size_t in) noexcept {
    // reallocate
    if(in > capacitor) {
        if(!reallocate(in)) {
            return false;
        }
    }

    // fill
    if(in > counter) {
        for(index_t i = counter; i < in; ++i) {
            container[relative(i)] = T();
        }
    }
    counter = in; // set

    return true;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::reserve(size_t in) noexcept {
    if(in < capacitor) {
        return true;
    }
    return reallocate(in);
}

template<typename T, size_t SVO> bool Deque<T, SVO>::compact() noexcept {
    return reallocate(counter);
}

template<typename T, size_t SVO> void Deque<T, SVO>::clear() noexcept {
    for(index_t i = 0; i < counter; ++i) {
        container[relative(i)].~T();
    }

    counter = 0;
    head    = 0;
    tail    = 0;
}

template<typename T, size_t SVO> size_t Deque<T, SVO>::size() const noexcept {
    return counter;
}

template<typename T, size_t SVO> size_t Deque<T, SVO>::capacity() const noexcept {
    return capacitor;
}

template<typename T, size_t SVO> inline bool Deque<T, SVO>::full() const noexcept {
    return counter == capacitor;
}

template<typename T, size_t SVO> inline bool Deque<T, SVO>::empty() const noexcept {
    return counter == 0;
}

template<typename T, size_t SVO> T* Deque<T, SVO>::data() const noexcept {
    return container;
}

template<typename T, size_t SVO> T& Deque<T, SVO>::at(index_t in) const {
    if(in < 0 || in >= counter) {
        throw std::out_of_range("index out of range");
    }
    return container[relative(in)];
}

template<typename T, size_t SVO> auto Deque<T, SVO>::begin() const noexcept -> Iterator {
    return Iterator{ this, 0 };
}

template<typename T, size_t SVO> auto Deque<T, SVO>::end() const noexcept -> Iterator {
    return Iterator{ this, counter }; // overflow safe: bitwise
}

template<typename T, size_t SVO> auto Deque<T, SVO>::rbegin() const noexcept -> Reverser {
    return Iterator{ this, counter - 1 }; // overflow safe: bitwise
}

template<typename T, size_t SVO> auto Deque<T, SVO>::rend() const noexcept -> Reverser {
    return Reverser{ this, -1 };
}

template<typename T, size_t SVO> auto Deque<T, SVO>::front() const noexcept -> Iterator {
    return Iterator{ begin() };
}

template<typename T, size_t SVO> auto Deque<T, SVO>::rear() const noexcept -> Reverser {
    return Reverser{ rbegin() };
}

template<typename T, size_t SVO> auto Deque<T, SVO>::top() const noexcept -> Reverser {
    return Reverser{ rbegin() };
}

template<typename T, size_t SVO> auto Deque<T, SVO>::bottom() const noexcept -> Iterator {
    return Iterator{ begin() };
}

template<typename T, size_t SVO> template<typename U> void Deque<T, SVO>::push_back(U&& in) {
    emplace(0, std::forward<U>(in));
}

template<typename T, size_t SVO> template<typename U> void Deque<T, SVO>::push_front(U&& in) {
    unshift(std::forward<U>(in));
}

template<typename T, size_t SVO> void Deque<T, SVO>::pop_back() {
    pop();
}

template<typename T, size_t SVO> void Deque<T, SVO>::pop_front() {
    shift();
}

template<typename T, size_t SVO> index_t Deque<T, SVO>::forward(index_t in) noexcept {
    return (in - 1) & (capacitor - 1);
}

template<typename T, size_t SVO> index_t Deque<T, SVO>::backward(index_t in) noexcept {
    return (in + 1) & (capacitor - 1);
}

template<typename T, size_t SVO> index_t Deque<T, SVO>::relative(index_t in) const noexcept {
    return (head + in) & (capacitor - 1);
}

template<typename T, size_t SVO> index_t Deque<T, SVO>::clamp(index_t in) const noexcept {
    index_t temp = (in % counter + counter);
    return temp >= counter ? temp - counter : temp; // return temp % counter
}

template<typename T, size_t SVO> bool Deque<T, SVO>::reallocate(size_t in) noexcept {
    in = Common::align(in);
    if(in == capacitor) {
        return true;
    }

    // limit
    if(in >= INT64_MAX) {
        return false;
    }

    T* newly = stack; // use stack: in <= MIN
    if(in > MIN) {
        newly = static_cast<T*>(malloc(sizeof(T) * in));
        if(!newly) {
            return false;
        }
    }

    // move
    index_t index = 0;
    for(index_t i = 0; i < counter; ++i) {
        if(index < static_cast<index_t>(in)) {
            newly[index++] = std::move(container[relative(i)]); // move
        } else container[relative(i)].~T();                     // delete
    }

    // free
    if(container != stack) {
        if (capacitor > MIN) {
            free(container);
        }
        else container = newly; // ctor not called
    }

    // set
    container = newly;
    capacitor = in < MIN ? MIN : in;
    head      = 0;
    tail      = counter - 1;
    return true;
}

template<typename T, size_t SVO>
Deque<T, SVO>::Iterator::Iterator(const Deque* in, index_t idx) noexcept: outer(in), index(idx) {}

template<typename T, size_t SVO>
Deque<T, SVO>::Iterator::Iterator(const Iterator& in) noexcept: outer(in.outer), index(in.index) {}

template<typename T, size_t SVO> Deque<T, SVO>::Iterator::Iterator(const Reverser& in) noexcept: Iterator(in) {}

template<typename T, size_t SVO> auto Deque<T, SVO>::Iterator::operator=(const Iterator& in) noexcept -> Iterator& {
    outer = in.outer;
    index = in.index;
    return *this;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Iterator::operator=(const Reverser& in) noexcept -> Iterator& {
    *this = in.iterator;
    return *this;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Iterator::operator++() noexcept -> Iterator& {
    ++index;
    return *this;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Iterator::operator--() noexcept -> Iterator& {
    --index;
    return *this;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Iterator::operator++(int) noexcept -> Iterator {
    Iterator temp = *this;
    ++*this;
    return temp;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Iterator::operator--(int) noexcept -> Iterator {
    Iterator temp = *this;
    --*this;
    return temp;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Iterator::operator+(index_t in) const noexcept -> Iterator {
    return Iterator(outer, index + in);
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Iterator::operator-(index_t in) const noexcept -> Iterator {
    return Iterator(outer, index - in);
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Iterator::operator+=(index_t in) noexcept -> Iterator& {
    *this = *this + in;
    return *this;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Iterator::operator-=(index_t in) noexcept -> Iterator& {
    *this = *this - in;
    return *this;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Iterator::operator==(const Iterator& in) const noexcept {
    return index == in.index;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Iterator::operator!=(const Iterator& in) const noexcept {
    return !(*this == in);
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Iterator::operator==(const Reverser& in) const noexcept {
    return *this == in.iterator;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Iterator::operator!=(const Reverser& in) const noexcept {
    return *this != in.iterator;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Iterator::operator<(const Iterator& in) const noexcept {
    return index < in.index;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Iterator::operator>(const Iterator& in) const noexcept {
    return index > in.index;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Iterator::operator>=(const Iterator& in) const noexcept {
    return index >= in.index;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Iterator::operator<=(const Iterator& in) const noexcept {
    return index <= in.index;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Iterator::operator<(const Reverser& in) const noexcept {
    return *this < in.iterator;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Iterator::operator>(const Reverser& in) const noexcept {
    return *this > in.iterator;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Iterator::operator<=(const Reverser& in) const noexcept {
    return *this <= in.iterator;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Iterator::operator>=(const Reverser& in) const noexcept {
    return *this >= in.iterator;
}

template<typename T, size_t SVO> T& Deque<T, SVO>::Iterator::operator*() const noexcept {
    return outer->container[outer->relative(outer->clamp(index))];
}

template<typename T, size_t SVO> T* Deque<T, SVO>::Iterator::operator->() const noexcept {
    return &**this;
}

template<typename T, size_t SVO>
Deque<T, SVO>::Reverser::Reverser(const Deque* in, index_t idx) noexcept: iterator(in, idx) {}

template<typename T, size_t SVO>
Deque<T, SVO>::Reverser::Reverser(const Reverser& in) noexcept: iterator(in.iterator) {}

template<typename T, size_t SVO> Deque<T, SVO>::Reverser::Reverser(const Iterator& in) noexcept: iterator(in) {}

template<typename T, size_t SVO> auto Deque<T, SVO>::Reverser::operator=(const Reverser& in) noexcept -> Reverser& {
    iterator = in.iterator;
    return *this;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Reverser::operator=(const Iterator& in) noexcept -> Reverser& {
    iterator = in;
    return *this;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Reverser::operator++() noexcept -> Reverser& {
    --iterator; // reverse
    return *this;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Reverser::operator--() noexcept -> Reverser& {
    ++iterator; // reverse
    return *this;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Reverser::operator++(int) noexcept -> Reverser {
    Reverser temp = *this;
    ++*this;
    return temp;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Reverser::operator--(int) noexcept -> Reverser {
    Reverser temp = *this;
    --*this;
    return temp;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Reverser::operator+(index_t in) const noexcept -> Reverser {
    return Reverser{ iterator - in }; // reverse
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Reverser::operator-(index_t in) const noexcept -> Reverser {
    return Reverser{ iterator + in }; // reverse
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Reverser::operator+=(index_t in) noexcept -> Reverser& {
    *this = *this + in;
    return *this;
}

template<typename T, size_t SVO> auto Deque<T, SVO>::Reverser::operator-=(index_t in) noexcept -> Reverser& {
    *this = *this - in;
    return *this;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Reverser::operator==(const Reverser& in) const noexcept {
    return iterator == in.iterator;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Reverser::operator!=(const Reverser& in) const noexcept {
    return iterator != in.iterator;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Reverser::operator==(const Iterator& in) const noexcept {
    return iterator == in;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Reverser::operator!=(const Iterator& in) const noexcept {
    return iterator != in;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Reverser::operator<(const Reverser& in) const noexcept {
    return iterator > in.iterator;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Reverser::operator>(const Reverser& in) const noexcept {
    return iterator < in.iterator;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Reverser::operator<=(const Reverser& in) const noexcept {
    return iterator >= in.iterator;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Reverser::operator>=(const Reverser& in) const noexcept {
    return iterator <= in.iterator;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Reverser::operator<(const Iterator& in) const noexcept {
    return iterator > in;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Reverser::operator>(const Iterator& in) const noexcept {
    return iterator < in;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Reverser::operator<=(const Iterator& in) const noexcept {
    return iterator >= in;
}

template<typename T, size_t SVO> bool Deque<T, SVO>::Reverser::operator>=(const Iterator& in) const noexcept {
    return iterator <= in;
}

template<typename T, size_t SVO> T& Deque<T, SVO>::Reverser::operator*() const noexcept {
    return *iterator;
}

template<typename T, size_t SVO> T* Deque<T, SVO>::Reverser::operator->() const noexcept {
    return &*iterator;
}

} // namespace stl

LWE_END
#endif