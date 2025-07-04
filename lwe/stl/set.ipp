LWE_BEGIN
namespace stl {

template<typename T> class Iterator<FWD, Set<T>> {
    ITERATOR_BODY(FWD, Set, T);
    using Bucket = typename Set::Bucket;

public:
    Iterator(Set* self, size_t index): self(self), index(index), chain(0), chaining(false) { }
    Iterator(Set* self, size_t index, uint8_t chain): self(self), index(index), chain(chain), chaining(true) { }

public:
    Iterator& operator++() {
        // find valid data, in range
        while(index != self->capacitor) {
            // bucket data -> chain data (index 0 -> 1)
            if(chaining == false) {
                chaining = true; // 0 -> 1

                // break when has data
                if(self->buckets[index].size != 0) {
                    break; // break for return bucket[0].chain[0]
                }
            }
            // in chain
            else {
                ++chain; // next
                // reached the end of chain
                if(chain >= self->buckets[index].size) {
                    chain    = 0;     // init
                    chaining = false; // index to 0 (chain -> bucket)
                    ++index;          // next bucket

                    // break when bucket not empty
                    if(self->buckets[index].used == true) {
                        break;
                    }
                }
                // haven't reached the end yet.
                else break;
            }
        } // end while

        return *this;
    }

    Iterator& operator--() {
        // [0][0]
        if(index == 0 && !chaining) {
            return *this;
        }

        // find valid data, in range
        do {
            // bucket -> chain
            if(chaining == false) {
                --index; // prev

                // has data
                if(self->buckets[index].used == true) {
                    // has chain
                    if(self->buckets[index].size != 0) {
                        chain    = self->buckets[index].size - 1; // last chain
                        chaining = true;                          // mark in chain
                    }
                    else chaining = false; // not chain
                    break;
                }
            }

            // in chain
            else {
                if(chain == 0) {
                    chaining = false; // chain -> bucket
                }
                else --chain; // prev chain
                break;
            }
        }
        while(index || chaining);
        return *this;
    }

public:
    T& operator*() {
        Bucket& bucket = self->buckets[index];
        if(chaining) {
            return bucket.chain[chain].data; // return chain data
        }
        else return bucket.data; // not chaining: return bucket data
    }

    const T& operator*() const { return const_cast<Iterator*>(this)->operator*(); }

    T* operator->() { return &this->operator*(); }

    const T* operator->() const { return &this->operator*(); }

public:
    bool operator==(const Iterator& in) const {
        return self == in.self && index == in.index && chain == in.chain && chaining == in.chaining;
    }

    bool operator!=(const Iterator& in) const { return !operator==(in); }

private:
    Set*    self;
    size_t  index;
    uint8_t chain;
    bool    chaining; // false == index, true == chain
};

template<typename T> Set<T>::~Set() {
    for(size_t i = 0; i < capacitor; ++i) {
        // delete bucket data
        if(buckets[i].used == true) {
            buckets[i].data.~T(); // bucket dtor
        }
        // has chain
        if(buckets[i].chain) {
            for(uint8_t j = 0; j < buckets[i].size; ++j) {
                // MSVC C6001 FALSE POSITIVE
                buckets[i].chain[j].data.~T(); // chain dtor
            }
            std::free(buckets[i].chain); // delete
        }
    }
    std::free(buckets);
}

template<typename T> bool Set<T>::resize() {
    size_t size = 0;
    if(capacitor) {
        size = capacitor << 1; // x2
        if(size < capacitor) {
            return false; // overflow
        }
        ++log; // log2(capacity)
    }
    else size = (size_t(1) << log); // init

    // realloc
    Bucket* old = buckets; // backup
    buckets     = static_cast<Bucket*>(std::malloc(sizeof(Bucket) * size));
    if(!buckets) {
        buckets = old; // rollback
        return false;  // bad alloc
    }

    // initialize
    for(size_t i = 0; i < size; ++i) {
        buckets[i].used     = false;
        buckets[i].chain    = nullptr;
        buckets[i].size     = 0;
        buckets[i].capacity = 0;
    }
    counter = 0; // reset

    // has data
    if(old) {
        // move
        for(size_t i = 0; i < capacitor; ++i) {
            // main data
            if(old[i].used == true) {
                insert(std::move(old[i].data), old[i].hash); // move
                old[i].data.~T();                            // delete
            }
            // chained datas
            if(old[i].chain) {
                for(uint8_t j = 0; j < old[i].size; ++j) {
                    Chain& chain = old[i].chain[j];
                    insert(std::move(chain.data), chain.hash); // move
                    chain.data.~T();                           // delete
                }
                // MSVC C6001 FALSE POSITIVE
                std::free(old[i].chain); // delete
            }
        }
        std::free(old); // delete
    }

    capacitor = size;
    return true;
}

template<typename T> size_t Set<T>::size() const noexcept {
    return counter;
}

template<typename T> size_t Set<T>::capacity() const noexcept {
    return capacitor;
}

template<typename T> auto Set<T>::bucket(size_t in) -> Bucket* {
    if(in >= capacitor) {
        throw diag::error(diag::OUT_OF_RANGE);
    }
    return buckets + in;
}

template<typename T> auto Set<T>::bucket(size_t in) const -> const Bucket* {
    return const_cast<Set*>(this)->bucket(in);
}

template<typename T> bool Set<T>::push(T&& in) {
    return insert(std::move(in));
}

template<typename T> bool Set<T>::push(const T& in) {
    return insert(in);
}

template<typename T> bool Set<T>::pop(const T& in) {
    hash_t hashed = util::hashof<T>(in); // get hash
    size_t index  = indexing(hashed);    // get index

    Bucket& bucket = buckets[index];

    // check first data: perform hash comparison first
    if(bucket.hash == hashed && bucket.data == in) {
        // has chain, swap and deled
        if(bucket.size != 0) {
            bucket.size -= 1;                         // reduce
            Chain& last  = bucket.chain[bucket.size]; // get last
            bucket.data  = std::move(last.data);      // move
            last.data.~T();                           // delete
        }
        // delete
        else {
            bucket.used = false; // mark
            bucket.data.~T();    // delete
        }
        --counter;   // count
        return true; // succeeded
    }

    // else find chain
    uint8_t size = bucket.size;
    for(uint8_t i = 0; i < size; ++i) {
        Chain& chain = bucket.chain[i];
        // same logic
        if(chain.hash == hashed && chain.data == in) {
            --bucket.size; // count
            --counter;     // total count

            // swap and delete
            if(bucket.size != 0) {
                Chain& last = bucket.chain[size - 1]; // get last
                chain.data  = std::move(last.data);   // move
                chain.hash  = last.hash;              // copy hash
                last.data.~T();                       // delete
            }
            else chain.data.~T(); // delete
            return true;          // succeeded
        }
    }
    return false; // not found
}

template<typename T> bool Set<T>::exist(const T& in) {
    return find(in) != end();
}

template<typename T> bool Set<T>::exist(hash_t in) {
    return find(in) != end();
}

template<typename T> auto Set<T>::find(const T& in) noexcept -> Iterator<FWD> {
    hash_t hashed = util::hashof<T>(in); // get hash
    size_t index  = indexing(hashed);    // get index

    Bucket& bucket = buckets[index];

    // check has data
    if(bucket.used == true) {
        // not chain, or first data
        if(bucket.hash == hashed && bucket.data == in) {
            return Iterator(this, index);
        }
        // return chain
        for(uint8_t i = 0; i < bucket.size; ++i) {
            Chain& chain = bucket.chain[i];
            if(chain.hash == hashed && chain.data == in) {
                return Iterator(this, index, i);
            }
        }
    }
    return end(); // not found
}

template<typename T> auto Set<T>::find(hash_t in) noexcept -> Iterator<FWD> {
    size_t index = indexing(in); // to index
    if(buckets[index].used == true) {
        return Iterator<FWD>(this, index); // first data
    }
    return end(); // not exist
}

template<typename T> auto Set<T>::at(size_t index) noexcept -> Iterator<FWD> {
    size_t pass = 0;
    // out of range
    if(index >= counter) {
        return end();
    }

    // loop
    for(size_t i = 0; i < capacitor; ++i) {
        // empty bucket pass
        if(buckets[i].used == false) {
            continue;
        }
        // check bucket pos
        if(pass == index) {
            return Iterator<FWD>(this, i);
        }
        pass += 1; // bucket pass

        // check chain
        if(buckets[i].size == 0) {
            continue; // not exist
        }

        // e.g. data
        // 0 [0] -> size 0:
        // 1 [1] -> size 3: [2][3][4]
        //                   ^--------index 2
        // pass 2 size 3
        // 2 + 3 == 5
        // index - pass -> 2 - 2 == 0
        // [1][0] == 2
        size_t size = buckets[i].size;
        if(index < (pass + size)) {
            return Iterator<FWD>(this, i, uint8_t(index - pass));
        }
        pass += size; // chain pass
    } // end for

    // ! MUST NOT BE ENTER THIS CODE: range check was performed. !
    return end();
}

template<typename T> auto Set<T>::begin() noexcept -> Iterator<FWD> {
    size_t index = 0;
    for(; index < capacitor; ++index) {
        if(buckets[index].used == true) {
            break;
        }
    }
    return Iterator<FWD>(this, index);
}

template<typename T> auto Set<T>::end() noexcept -> Iterator<FWD> {
    return Iterator<FWD>(this, capacitor);
}

template<typename T>
template<typename U> bool Set<T>::insert(U&& in) {
    // size check
    if(counter >= factor) {
        if(!resize()) {
            return false; // bad alloc
        }
        factor = size_t(float(capacitor) * LOAD_FACTOR);
    }
    hash_t hash = util::hashof<T>(in); // const T&
    return insert(std::forward<U>(in), hash);
}

template<typename T>
template<typename U> bool Set<T>::insert(U&& in, hash_t hashed) {
    size_t index = indexing(hashed); // to index

    Bucket& bucket = buckets[index];

    // duplicated check
    if(bucket.used == true) {
        if(bucket.data == in) {
            return false;
        }
        for(uint8_t i = 0; i < bucket.size; ++i) {
            if(bucket.chain[i].data == in) {
                return false;
            }
        }
    }

    // false -> empty
    if(bucket.used == false) {
        // insert
        new(&bucket.data) T(std::forward<U>(in)); // copy or move, can throw
        bucket.used = true;                       // check
        bucket.hash = hashed;                     // store
        ++counter;                                // count
        return true;
    }

    // else true -> use chain
    uint8_t size = bucket.size;     // get size
    uint8_t cap  = bucket.capacity; // get capacity

    // check realloc condition
    if(size >= cap) {
        // ! CHAIN SIZE INCREASES BY 1: chains are allocated less frequently !
        ++cap;
        Chain* newly = static_cast<Chain*>(std::malloc((cap) * sizeof(Chain)));
        if(!newly) {
            return false; // bad alloc
        }

        // remove old and move
        Chain* old = bucket.chain;
        for(uint8_t i = 0; i < size; ++i) {
            new(&newly[i].data) T(std::move(old[i].data)); // move, maybe noexcept
            newly[i].hash = old[i].hash;                   // copy hash
            old[i].data.~T();                              // dtor
        }
        bucket.chain     = newly; // store
        bucket.capacity += 1;     // count
        std::free(old);           // delete
    }

    // isnert data
    new(&bucket.chain[size].data) T(std::forward<U>(in)); // copy or move, can throw
    bucket.chain[size].hash = hashed;                     // insert hash

    ++bucket.size; // size
    ++counter;     // total size
    return true;
}

template<typename T> auto Set<T>::find(const T& in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Set*>(this)->find(in);
}

template<typename T> auto Set<T>::find(hash_t in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Set*>(this)->find(in);
}

template<typename T> auto Set<T>::at(size_t in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Set*>(this)->at(in);
}

template<typename T> auto Set<T>::begin() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Set*>(this)->begin();
}

template<typename T> auto Set<T>::end() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Set*>(this)->end();
}

REGISTER_CONST_ITERATOR((typename T), FWD, Set, T);

} // namespace stl
LWE_END
