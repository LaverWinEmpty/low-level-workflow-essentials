LWE_BEGIN
namespace container {

/**************************************************************************************************
 * Iterator Specialization
 **************************************************************************************************/

template<typename T, size_t SVO> class Iterator<FWD, Vector<T, SVO>> {
    ITERATOR_BODY(FWD, Vector, T, SVO);
public:
    Iterator(Vector*, index_t) noexcept;
    Iterator(const Iterator&) noexcept;
    Iterator(const Reverse&) noexcept;
    Iterator& operator=(const Iterator&) noexcept;
    Iterator& operator=(const Iterator<BWD, Vector>&) noexcept;
    Iterator& operator++() noexcept;
    Iterator& operator--() noexcept;
    Iterator  operator++(int) noexcept;
    Iterator  operator--(int) noexcept;
    Iterator  operator+(index_t) const noexcept;
    Iterator  operator-(index_t) const noexcept;
    Iterator& operator+=(index_t) noexcept;
    Iterator& operator-=(index_t) noexcept;
    bool     operator==(const Iterator&) const noexcept;
    bool     operator!=(const Iterator&) const noexcept;
    bool     operator==(const Reverse&) const noexcept;
    bool     operator!=(const Reverse&) const noexcept;
    bool     operator<(const Iterator&) const noexcept;
    bool     operator>(const Iterator&) const noexcept;
    bool     operator<=(const Iterator&) const noexcept;
    bool     operator>=(const Iterator&) const noexcept;
    bool     operator<(const Reverse&) const noexcept;
    bool     operator>(const Reverse&) const noexcept;
    bool     operator<=(const Reverse&) const noexcept;
    bool     operator>=(const Reverse&) const noexcept;
    const T& operator*() const noexcept;
    const T* operator->() const noexcept;
    T&       operator*() noexcept;
    T*       operator->() noexcept;
private:
    Vector* outer;
    index_t index;
};

template<typename T, size_t SVO> class Iterator<BWD, Vector<T, SVO>> {
    ITERATOR_BODY(BWD, Vector, T, SVO);
public:
    Iterator(Vector*, index_t) noexcept;
    Iterator(const Iterator&) noexcept;
    Iterator(const Reverse&) noexcept;
    Iterator& operator=(const Iterator&) noexcept;
    Iterator& operator=(const Reverse&) noexcept;
    Iterator& operator++() noexcept;
    Iterator& operator--() noexcept;
    Iterator  operator++(int) noexcept;
    Iterator  operator--(int) noexcept;
    bool      operator==(const Iterator&) const noexcept;
    bool      operator!=(const Iterator&) const noexcept;
    bool      operator==(const Reverse&) const noexcept;
    bool      operator!=(const Reverse&) const noexcept;
    bool      operator<(const Iterator&) const noexcept;
    bool      operator>(const Iterator&) const noexcept;
    bool      operator<=(const Iterator&) const noexcept;
    bool      operator>=(const Iterator&) const noexcept;
    bool      operator<(const Reverse&) const noexcept;
    bool      operator>(const Reverse&) const noexcept;
    bool      operator<=(const Reverse&) const noexcept;
    bool      operator>=(const Reverse&) const noexcept;
    Iterator  operator+(index_t) const noexcept;
    Iterator  operator-(index_t) const noexcept;
    Iterator& operator+=(index_t) noexcept;
    Iterator& operator-=(index_t) noexcept;
    const T& operator*() const noexcept;
    const T* operator->() const noexcept;
    T&       operator*() noexcept;
    T*       operator->() noexcept;
private:
    Reverse iterator;
};

REGISTER_CONST_ITERATOR((typename T, size_t SVO), FWD, Vector, T, SVO);
REGISTER_CONST_ITERATOR((typename T, size_t SVO), BWD, Vector, T, SVO);

/**************************************************************************************************
 * Vector
 **************************************************************************************************/

template<typename T, size_t SVO> Vector<T, SVO>::Vector() { }

template<typename T, size_t SVO> Vector<T, SVO>::~Vector() {
    clear();
    if(container != stack) {
        free(container);
    }
}

template<typename T, size_t SVO> T& Vector<T, SVO>::operator[](index_t index) const noexcept {
    return container[relative(index)];
}

template<typename T, size_t SVO> template<size_t N> Vector<T, SVO>::Vector(const Vector<T, N>& in) {
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

template<typename T, size_t SVO> template<size_t N> Vector<T, SVO>::Vector(Vector<T, N>&& in) {
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
        counter   = in.counter;
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

template<typename T, size_t SVO> template<size_t N> Vector<T, N>& Vector<T, SVO>::operator=(const Vector<T, N>& in) {
    clear();

    if(!reallocate(in.counter)) {
        throw std::bad_alloc();
    }

    for(size_t i = 0; i < in.counter; ++i) {
        container[i] = in.container[relative(i)];
    }

    return *this;
}

template<typename T, size_t SVO> template<size_t N> Vector<T, N>& Vector<T, SVO>::operator=(Vector<T, N>&& in) {
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
            counter   = in.counter;
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

template<typename T, size_t SVO> template<typename Arg> bool Vector<T, SVO>::emplace(index_t index, Arg&& in) noexcept {
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
    // move for insert in the middle
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

template<typename T, size_t SVO> bool Vector<T, SVO>::erase(index_t index, T* out) noexcept {
    if(index < 0 || index >= counter) {
        return false;
    }

    if(out) {
        *out = std::move(container[relative(index)]);
    }
    else container[relative(index)].~T();
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

template<typename T, size_t SVO> bool Vector<T, SVO>::push() noexcept {
    return emplace(counter, T{});
}

template<typename T, size_t SVO> bool Vector<T, SVO>::shift() noexcept {
    return emplace(-1, T{});
}

template<typename T, size_t SVO> bool Vector<T, SVO>::push(T&& in) noexcept {
    return emplace(counter, std::move(in));
}

template<typename T, size_t SVO> bool Vector<T, SVO>::push(const T& in) noexcept {
    return emplace(counter, in);
}

template<typename T, size_t SVO> bool Vector<T, SVO>::shift(T&& in) noexcept {
    return emplace(-1, std::move(in));
}

template<typename T, size_t SVO> bool Vector<T, SVO>::shift(const T& in) noexcept {
    return emplace(-1, in);
}

template<typename T, size_t SVO> bool Vector<T, SVO>::pop(T* out) noexcept {
    return erase(counter - 1, out);
}

template<typename T, size_t SVO> bool Vector<T, SVO>::pop(T& out) noexcept {
    return erase(counter - 1, &out);
}

template<typename T, size_t SVO> bool Vector<T, SVO>::pull(T* out) noexcept {
    return erase(0, out);
}

template<typename T, size_t SVO> bool Vector<T, SVO>::pull(T& out) noexcept {
    return erase(0, &out);
}

template<typename T, size_t SVO> bool Vector<T, SVO>::resize(size_t in) noexcept {
    // reallocate
    if(in > capacitor) {
        if(!reallocate(in)) {
            return false;
        }
    }

    // fill
    if(in > counter) {
        for(index_t i = counter; i < in; ++i) {
            new(container + relative(i)) T{};
        }
    }
    counter = in; // set

    return true;
}

template<typename T, size_t SVO> bool Vector<T, SVO>::reserve(size_t in) noexcept {
    if(in < capacitor) {
        return true;
    }
    return reallocate(in);
}

template<typename T, size_t SVO> bool Vector<T, SVO>::compact() noexcept {
    return reallocate(counter);
}

template<typename T, size_t SVO> void Vector<T, SVO>::clear() noexcept {
    for(index_t i = 0; i < counter; ++i) {
        container[relative(i)].~T();
    }

    counter = 0;
    head    = 0;
    tail    = 0;
}

template<typename T, size_t SVO> size_t Vector<T, SVO>::size() const noexcept {
    return counter;
}

template<typename T, size_t SVO> size_t Vector<T, SVO>::capacity() const noexcept {
    return capacitor;
}

template<typename T, size_t SVO> inline bool Vector<T, SVO>::full() const noexcept {
    return counter == capacitor;
}

template<typename T, size_t SVO> inline bool Vector<T, SVO>::empty() const noexcept {
    return counter == 0;
}

template<typename T, size_t SVO> T* Vector<T, SVO>::data() noexcept {
    return container;
}

template<typename T, size_t SVO> const T* Vector<T, SVO>::data() const noexcept {
    return const_cast<Vector*>(this)->data();
}

template<typename T, size_t SVO> T& Vector<T, SVO>::at(index_t in) {
    if(in < 0 || in >= counter) {
        throw diag::error(diag::OUT_OF_RANGE);
    }
    return container[relative(in)];
}

template<typename T, size_t SVO> const T& Vector<T, SVO>::at(index_t in) const {
    return const_cast<Vector*>(this)->at(in);
}

template<typename T, size_t SVO> auto Vector<T, SVO>::begin() noexcept -> Iterator<FWD> {
    return Iterator<FWD | VIEW>{ this, 0 };
}

template<typename T, size_t SVO> auto Vector<T, SVO>::end() noexcept -> Iterator<FWD> {
    return Iterator<FWD | VIEW>{ this, counter }; // overflow safe: bitwise
}

template<typename T, size_t SVO> auto Vector<T, SVO>::rbegin() noexcept -> Iterator<BWD> {
    return Iterator<BWD | VIEW>{ this, counter - 1 }; // overflow safe: bitwise
}

template<typename T, size_t SVO> auto Vector<T, SVO>::rend() noexcept -> Iterator<BWD> {
    return Iterator<BWD | VIEW>{ this, -1 };
}

template<typename T, size_t SVO> auto Vector<T, SVO>::front() noexcept -> Iterator<FWD> {
    return Iterator<FWD | VIEW>{ begin() };
}

template<typename T, size_t SVO> auto Vector<T, SVO>::rear() noexcept -> Iterator<BWD> {
    return Iterator<BWD | VIEW>{ rbegin() };
}

template<typename T, size_t SVO> auto Vector<T, SVO>::top() noexcept -> Iterator<BWD> {
    return Iterator<BWD | VIEW>{ rbegin() };
}

template<typename T, size_t SVO> auto Vector<T, SVO>::bottom() noexcept -> Iterator<FWD> {
    return Iterator<FWD | VIEW>{ begin() };
}

template<typename T, size_t SVO> auto Vector<T, SVO>::begin() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Vector*>(this)->begin();
}

template<typename T, size_t SVO> auto Vector<T, SVO>::end() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Vector*>(this)->end();
}

template<typename T, size_t SVO> auto Vector<T, SVO>::rbegin() const noexcept -> Iterator<BWD | VIEW> {
    return const_cast<Vector*>(this)->rbegin();
}

template<typename T, size_t SVO> auto Vector<T, SVO>::rend() const noexcept -> Iterator<BWD | VIEW> {
    return const_cast<Vector*>(this)->rend();
}

template<typename T, size_t SVO> auto Vector<T, SVO>::front() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Vector*>(this)->front();
}

template<typename T, size_t SVO> auto Vector<T, SVO>::rear() const noexcept -> Iterator<BWD | VIEW> {
    return const_cast<Vector*>(this)->rear();
}

template<typename T, size_t SVO> auto Vector<T, SVO>::top() const noexcept -> Iterator<BWD | VIEW> {
    return const_cast<Vector*>(this)->top();
}

template<typename T, size_t SVO> auto Vector<T, SVO>::bottom() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Vector*>(this)->bottom();
}

template<typename T, size_t SVO> template<typename U> void Vector<T, SVO>::push_back(U&& in) {
    emplace(0, std::forward<U>(in));
}

template<typename T, size_t SVO> template<typename U> void Vector<T, SVO>::push_front(U&& in) {
    shift(std::forward<U>(in));
}

template<typename T, size_t SVO> void Vector<T, SVO>::pop_back() {
    push();
}

template<typename T, size_t SVO> void Vector<T, SVO>::pop_front() {
    shift();
}

template<typename T, size_t SVO> index_t Vector<T, SVO>::forward(index_t in) noexcept {
    return (in - 1) & (capacitor - 1);
}

template<typename T, size_t SVO> index_t Vector<T, SVO>::backward(index_t in) noexcept {
    return (in + 1) & (capacitor - 1);
}

template<typename T, size_t SVO> index_t Vector<T, SVO>::relative(index_t in) const noexcept {
    return (head + in) & (capacitor - 1);
}

template<typename T, size_t SVO> index_t Vector<T, SVO>::clamp(index_t in) const noexcept {
    index_t temp = (in % counter + counter);
    return temp >= counter ? temp - counter : temp; // return temp % counter
}

template<typename T, size_t SVO> bool Vector<T, SVO>::reallocate(size_t in) noexcept {
    if(in < config::SMALLVECTOR) {
        in = config::SMALLVECTOR; // set default min
    }
    else in = align(in);

    // no reallocation required
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
            new(newly + index) T(std::move(container[relative(i)])); // move
            ++index;
        }
        else container[relative(i)].~T(); // delete
    }

    // free
    if(container != stack) {
        if(capacitor > MIN) {
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

/**************************************************************************************************
 * Iterator
 **************************************************************************************************/

template<typename T, size_t SVO>
Iterator<FWD, Vector<T, SVO>>::Iterator(Vector* in, index_t idx) noexcept: outer(in), index(idx) { }

template<typename T, size_t SVO>
Iterator<FWD, Vector<T, SVO>>::Iterator(const Iterator& in) noexcept: outer(in.outer), index(in.index) { }

template<typename T, size_t SVO>
Iterator<FWD, Vector<T, SVO>>::Iterator(const Reverse& in) noexcept: Iterator(in.iterator) { }

template<typename T, size_t SVO>
auto Iterator<FWD, Vector<T, SVO>>::operator=(const Iterator& in) noexcept -> Iterator& {
    outer = in.outer;
    index = in.index;
    return *this;
}

template<typename T, size_t SVO>
auto Iterator<FWD, Vector<T, SVO>>::operator=(const Reverse& in) noexcept -> Iterator& {
    *this = in.iterator;
    return *this;
}

template<typename T, size_t SVO>
auto Iterator<FWD, Vector<T, SVO>>::operator++() noexcept -> Iterator& {
    ++index;
    return *this;
}

template<typename T, size_t SVO>
auto Iterator<FWD, Vector<T, SVO>>::operator--() noexcept -> Iterator& {
    --index;
    return *this;
}

template<typename T, size_t SVO>
auto Iterator<FWD, Vector<T, SVO>>::operator++(int) noexcept -> Iterator {
    Iterator<FWD> temp = *this;
    ++*this;
    return temp;
}

template<typename T, size_t SVO>
auto Iterator<FWD, Vector<T, SVO>>::operator--(int) noexcept -> Iterator {
    Iterator<FWD> temp = *this;
    --*this;
    return temp;
}

template<typename T, size_t SVO>
auto Iterator<FWD, Vector<T, SVO>>::operator+(index_t in) const noexcept -> Iterator {
    return Iterator(outer, index + in);
}

template<typename T, size_t SVO>
auto Iterator<FWD, Vector<T, SVO>>::operator-(index_t in) const noexcept -> Iterator {
    return Iterator(outer, index - in);
}

template<typename T, size_t SVO>
auto Iterator<FWD, Vector<T, SVO>>::operator+=(index_t in) noexcept -> Iterator& {
    *this = *this + in;
    return *this;
}

template<typename T, size_t SVO>
auto Iterator<FWD, Vector<T, SVO>>::operator-=(index_t in) noexcept -> Iterator& {
    *this = *this - in;
    return *this;
}

template<typename T, size_t SVO>
bool Iterator<FWD, Vector<T, SVO>>::operator==(const Iterator& in) const noexcept {
    return index == in.index;
}

template<typename T, size_t SVO>
bool Iterator<FWD, Vector<T, SVO>>::operator!=(const Iterator& in) const noexcept {
    return !(*this == in);
}

template<typename T, size_t SVO>
bool Iterator<FWD, Vector<T, SVO>>::operator==(const Reverse& in) const noexcept {
    return *this == in.iterator;
}

template<typename T, size_t SVO>
bool Iterator<FWD, Vector<T, SVO>>::operator!=(const Reverse& in) const noexcept {
    return *this != in.iterator;
}

template<typename T, size_t SVO>
bool Iterator<FWD, Vector<T, SVO>>::operator<(const Iterator& in) const noexcept {
    return index < in.index;
}

template<typename T, size_t SVO>
bool Iterator<FWD, Vector<T, SVO>>::operator>(const Iterator& in) const noexcept {
    return index > in.index;
}

template<typename T, size_t SVO>
bool Iterator<FWD, Vector<T, SVO>>::operator>=(const Iterator& in) const noexcept {
    return index >= in.index;
}

template<typename T, size_t SVO>
bool Iterator<FWD, Vector<T, SVO>>::operator<=(const Iterator& in) const noexcept {
    return index <= in.index;
}

template<typename T, size_t SVO>
bool Iterator<FWD, Vector<T, SVO>>::operator<(const Reverse& in) const noexcept {
    return *this < in.iterator;
}

template<typename T, size_t SVO>
bool Iterator<FWD, Vector<T, SVO>>::operator>(const Reverse& in) const noexcept {
    return *this > in.iterator;
}

template<typename T, size_t SVO>
bool Iterator<FWD, Vector<T, SVO>>::operator<=(const Reverse& in) const noexcept {
    return *this <= in.iterator;
}

template<typename T, size_t SVO>
bool Iterator<FWD, Vector<T, SVO>>::operator>=(const Reverse& in) const noexcept {
    return *this >= in.iterator;
}

template<typename T, size_t SVO>
const T& Iterator<FWD, Vector<T, SVO>>::operator*() const noexcept {
    return const_cast<Iterator*>(this)->operator*();
}

template<typename T, size_t SVO>
T& Iterator<FWD, Vector<T, SVO>>::operator*() noexcept {
    return outer->container[outer->relative(outer->clamp(index))];
}

template<typename T, size_t SVO>
const T* Iterator<FWD, Vector<T, SVO>>::operator->() const noexcept {
    return const_cast<Iterator*>(this)->operator->();
}

template<typename T, size_t SVO>
T* Iterator<FWD, Vector<T, SVO>>::operator->() noexcept {
    return &**this;
}

template<typename T, size_t SVO>
Iterator<BWD, Vector<T, SVO>>::Iterator(Vector* in, index_t idx) noexcept: iterator(in, idx) { }

template<typename T, size_t SVO>
Iterator<BWD, Vector<T, SVO>>::Iterator(const Iterator& in) noexcept: iterator(in.iterator) { }

template<typename T, size_t SVO>
Iterator<BWD, Vector<T, SVO>>::Iterator(const Reverse& in) noexcept: iterator(in) { }

template<typename T, size_t SVO>
auto Iterator<BWD, Vector<T, SVO>>::operator=(const Iterator& in) noexcept -> Iterator& {
    iterator = in.iterator;
    return *this;
}

template<typename T, size_t SVO>
auto Iterator<BWD, Vector<T, SVO>>::operator=(const Reverse& in) noexcept -> Iterator& {
    iterator = in;
    return *this;
}

template<typename T, size_t SVO>
auto Iterator<BWD, Vector<T, SVO>>::operator++() noexcept -> Iterator& {
    --iterator; // reverse
    return *this;
}

template<typename T, size_t SVO>
auto Iterator<BWD, Vector<T, SVO>>::operator--() noexcept -> Iterator& {
    ++iterator; // reverse
    return *this;
}

template<typename T, size_t SVO>
auto Iterator<BWD, Vector<T, SVO>>::operator++(int) noexcept -> Iterator {
    Iterator<BWD> temp = *this;
    ++*this;
    return temp;
}

template<typename T, size_t SVO>
auto Iterator<BWD, Vector<T, SVO>>::operator--(int) noexcept -> Iterator {
    Iterator<BWD> temp = *this;
    --*this;
    return temp;
}

template<typename T, size_t SVO>
auto Iterator<BWD, Vector<T, SVO>>::operator+(index_t in) const noexcept -> Iterator {
    return Iterator{ iterator - in }; // reverse
}

template<typename T, size_t SVO>
auto Iterator<BWD, Vector<T, SVO>>::operator-(index_t in) const noexcept -> Iterator {
    return Iterator{ iterator + in }; // reverse
}

template<typename T, size_t SVO>
auto Iterator<BWD, Vector<T, SVO>>::operator+=(index_t in) noexcept -> Iterator& {
    *this = *this + in;
    return *this;
}

template<typename T, size_t SVO>
auto Iterator<BWD, Vector<T, SVO>>::operator-=(index_t in) noexcept -> Iterator& {
    *this = *this - in;
    return *this;
}

template<typename T, size_t SVO>
bool Iterator<BWD, Vector<T, SVO>>::operator==(const Iterator& in) const noexcept {
    return iterator == in.iterator;
}

template<typename T, size_t SVO>
bool Iterator<BWD, Vector<T, SVO>>::operator!=(const Iterator& in) const noexcept {
    return iterator != in.iterator;
}

template<typename T, size_t SVO>
bool Iterator<BWD, Vector<T, SVO>>::operator==(const Reverse& in) const noexcept {
    return iterator == in;
}

template<typename T, size_t SVO>
bool Iterator<BWD, Vector<T, SVO>>::operator!=(const Reverse& in) const noexcept {
    return iterator != in;
}

template<typename T, size_t SVO>
bool Iterator<BWD, Vector<T, SVO>>::operator<(const Iterator& in) const noexcept {
    return iterator > in.iterator;
}

template<typename T, size_t SVO>
bool Iterator<BWD, Vector<T, SVO>>::operator>(const Iterator& in) const noexcept {
    return iterator < in.iterator;
}

template<typename T, size_t SVO>
bool Iterator<BWD, Vector<T, SVO>>::operator<=(const Iterator& in) const noexcept {
    return iterator >= in.iterator;
}

template<typename T, size_t SVO>
bool Iterator<BWD, Vector<T, SVO>>::operator>=(const Iterator& in) const noexcept {
    return iterator <= in.iterator;
}

template<typename T, size_t SVO>
bool Iterator<BWD, Vector<T, SVO>>::operator<(const Reverse& in) const noexcept {
    return iterator > in;
}

template<typename T, size_t SVO>
bool Iterator<BWD, Vector<T, SVO>>::operator>(const Reverse& in) const noexcept {
    return iterator < in;
}

template<typename T, size_t SVO>
bool Iterator<BWD, Vector<T, SVO>>::operator<=(const Reverse& in) const noexcept {
    return iterator >= in;
}

template<typename T, size_t SVO>
bool Iterator<BWD, Vector<T, SVO>>::operator>=(const Reverse& in) const noexcept {
    return iterator <= in;
}

template<typename T, size_t SVO>
const T& Iterator<BWD, Vector<T, SVO>>::operator*() const noexcept {
    return *iterator;
}

template<typename T, size_t SVO>
const T* Iterator<BWD, Vector<T, SVO>>::operator->() const noexcept {
    return &*iterator;
}

template<typename T, size_t SVO>
T& Iterator<BWD, Vector<T, SVO>>::operator*() noexcept {
    return *iterator;
}

template<typename T, size_t SVO>
T* Iterator<BWD, Vector<T, SVO>>::operator->() noexcept {
    return &*iterator;
}

} // namespace container
LWE_END
