LWE_BEGIN
namespace container {
/**************************************************************************************************
 * Iterator
 **************************************************************************************************/
REGISTER_CONST_ITERATOR((typename T), FWD, Hashtable, T);

template<typename T> class Iterator<FWD, Hashtable<T>> {
    ITERATOR_BODY(FWD, Hashtable, T);
    using Bucket = typename Hashtable::Bucket;

public:
    Iterator(Hashtable* self, size_t index): self(self), index(index), chain(0), chaining(false) { }
    Iterator(Hashtable* self, size_t index, uint16_t chain): self(self), index(index), chain(chain), chaining(true) { }

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

                    // end
                    if(index >= self->capacitor) {
                        break;
                    }
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
                // begin
                if(index == 0) {
                    break;
                }
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
    Hashtable* self;
    size_t     index;
    uint16_t   chain;
    bool       chaining; // false == index, true == chain
};

/**************************************************************************************************
 * Hashtable
 **************************************************************************************************/

template<typename T> Hashtable<T>::Hashtable(float factor, Grower grower): LOAD_FACTOR(factor), grower(grower) { }

template<typename T> Hashtable<T>::Hashtable(Grower grower): Hashtable(config::LOADFACTOR, grower) { }

template<typename T> Hashtable<T>::~Hashtable() {
    if(buckets) {
        clear();
        free(buckets);
    }
}

template<typename T> bool Hashtable<T>::resize(uint64_t caplog) {
    size_t size = (size_t(1) << caplog);
    if(size <= capacitor) {
        return false; // shrink not allow
    }

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
                emplace(std::move(old[i].data), old[i].hash); // move
                old[i].data.~T();                             // delete
            }
            // chained datas
            if(old[i].chain) {
                for(uint16_t j = 0; j < old[i].size; ++j) {
                    Chain& chain = old[i].chain[j];
                    emplace(std::move(chain.data), chain.hash); // move
                    chain.data.~T();                            // delete
                }
                // MSVC C6001 FALSE POSITIVE
                std::free(old[i].chain); // delete
            }
        }
        std::free(old); // delete
    }

    capacitor = size;
    log       = caplog;
    return true;
}

template<typename T> size_t Hashtable<T>::indexing(hash_t in) const noexcept {
    static constexpr size_t FIBONACCI_PRIME = []() {
        if constexpr(sizeof(size_t) == 8) {
            return 11'400'714'819'323'198'485ull;
        }
        else return 2'654'435'769u;
    }();
    return (in * FIBONACCI_PRIME) >> ((sizeof(size_t) << 3) - log);
}

template<typename T> size_t Hashtable<T>::size() const noexcept {
    return counter;
}

template<typename T> size_t Hashtable<T>::capacity() const noexcept {
    return capacitor;
}

template<typename T> auto Hashtable<T>::bucket(size_t in) const noexcept -> const Bucket* {
    if(in >= capacitor) {
        return nullptr;
    }
    return buckets + in;
}

template<typename T> bool Hashtable<T>::push(T&& in) {
    return emplace(std::move(in));
}

template<typename T> bool Hashtable<T>::push(const T& in) {
    return emplace(in);
}

template<typename T> bool Hashtable<T>::pop(const T& in) noexcept {
    hash_t hashed = util::hashof(in); // get hash
    size_t index  = indexing(hashed); // get index

    Bucket& bucket = buckets[index];

    // check first data: perform hash comparison first
    if(bucket.hash == hashed && bucket.data == in) {
        // has chain, swap and deled
        if(bucket.size != 0) {
            bucket.size -= 1;                         // reduce
            Chain& last  = bucket.chain[bucket.size]; // get last
            bucket.data  = std::move(last.data);      // move
            bucket.hash  = last.hash;                 // copy hash
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
    uint16_t size = bucket.size;
    for(uint16_t i = 0; i < size; ++i) {
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

template<typename T> bool Hashtable<T>::pop(const Iterator<FWD>& in) noexcept {
    if(in == end()) {
        return false;
    }
    return pop(*in); // delete
}

template<typename T> bool Hashtable<T>::pop(hash_t in) noexcept {
    Iterator<FWD> it = find(in); // find by hash
    if(it == end()) {
        return false;
    }
    return pop(*it); // delete
}

template<typename T> bool Hashtable<T>::exist(const T& in) noexcept {
    return find(in) != end();
}

template<typename T> bool Hashtable<T>::exist(hash_t in) noexcept {
    return find(in) != end();
}

template<typename T> bool Hashtable<T>::reserve(size_t in) noexcept {
    // algin to power of 2
    in = core::align(in);
    if(in <= capacitor) {
        return true; // already allocated
    }

    // calculate log2
    size_t inlog = 0;
    while(in > 1) {
        in >>= 1;
        ++inlog;
    }

    // resize
    if(!resize(inlog)) {
        return false; // rollback
    }
    return true;
}

template<typename T> void Hashtable<T>::clear() noexcept {
    if(capacitor != 0) {
        return;
    }

    for(size_t i = 0; i < capacitor; ++i) {
        // delete bucket data
        if(buckets[i].used == true) {
            buckets[i].data.~T(); // bucket dtor
        }
        // has chain
        if(buckets[i].chain) {
            for(uint16_t j = 0; j < buckets[i].size; ++j) {
                // MSVC C6001 FALSE POSITIVE
                buckets[i].chain[j].data.~T(); // chain dtor
            }
            std::free(buckets[i].chain); // delete
        }
    }

    // init
    // ignore bucket, capacity
    counter = 0;
    factor  = 0;
}

template<typename T> auto Hashtable<T>::find(const T& in) noexcept -> Iterator<FWD> {
    hash_t hashed = util::hashof(in); // get hash
    size_t index  = indexing(hashed); // get index

    Bucket& bucket = buckets[index];

    // check has data
    if(bucket.used == true) {
        // not chain, or first data
        if(bucket.hash == hashed && bucket.data == in) {
            return Iterator(this, index);
        }
        // return chain
        for(uint16_t i = 0; i < bucket.size; ++i) {
            Chain& chain = bucket.chain[i];
            if(chain.hash == hashed && chain.data == in) {
                return Iterator(this, index, i);
            }
        }
    }
    return end(); // not found
}

template<typename T> auto Hashtable<T>::find(hash_t in) noexcept -> Iterator<FWD> {
    size_t index = indexing(in); // to index
    if(buckets[index].used == true) {
        return Iterator<FWD>(this, index); // first data
    }
    return end(); // not exist
}

template<typename T> auto Hashtable<T>::at(size_t index) noexcept -> Iterator<FWD> {
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
            return Iterator<FWD>(this, i, uint16_t(index - pass));
        }
        pass += size; // chain pass
    } // end for

    // ! MUST NOT BE ENTER THIS CODE: range check was performed. !
    return end();
}

template<typename T> auto Hashtable<T>::begin() noexcept -> Iterator<FWD> {
    size_t index = 0;
    for(; index < capacitor; ++index) {
        if(buckets[index].used == true) {
            break;
        }
    }
    return Iterator<FWD>(this, index);
}

template<typename T> auto Hashtable<T>::end() noexcept -> Iterator<FWD> {
    return Iterator<FWD>(this, capacitor);
}

template<typename T>
template<typename U> bool Hashtable<T>::emplace(U&& in) {
    // size check
    if(counter >= factor) {
        if(!resize(log + 1)) {
            return false; // bad alloc
        }
        factor = size_t(float(capacitor) * LOAD_FACTOR);
    }
    hash_t hash = util::hashof(in); // const T&
    return emplace(std::forward<U>(in), hash);
}

template<typename T>
template<typename U> bool Hashtable<T>::emplace(U&& in, hash_t hashed, bool check) {
    size_t index = indexing(hashed); // to index

    Bucket& bucket = buckets[index];

    if(check) {
        // duplicated check
        if(bucket.used == true) {
            if(bucket.data == in) {
                return false;
            }
            for(uint16_t i = 0; i < bucket.size; ++i) {
                if(bucket.chain[i].data == in) {
                    return false;
                }
            }
        }
    }

    // false -> empty
    if(bucket.used == false) {
        // emplace
        new(&bucket.data) T(std::forward<U>(in)); // copy or move, can throw
        bucket.used = true;                       // check
        bucket.hash = hashed;                     // store
        ++counter;                                // count
        return true;
    }

    // else true -> use chain
    uint16_t size = bucket.size;     // get size
    uint16_t cap  = bucket.capacity; // get capacity

    // check realloc condition
    if(size >= cap) {
        cap          = grower(cap);
        Chain* newly = static_cast<Chain*>(std::malloc(sizeof(Chain) * cap));
        if(!newly) {
            return false; // bad alloc
        }

        // remove old and move
        Chain* old = bucket.chain;
        for(uint16_t i = 0; i < size; ++i) {
            new(&newly[i].data) T(std::move(old[i].data)); // move, maybe noexcept
            newly[i].hash = old[i].hash;                   // copy hash
            old[i].data.~T();                              // dtor
        }
        bucket.chain    = newly; // store
        bucket.capacity = cap;   // count
        std::free(old);          // delete
    }

    // isnert data
    new(&bucket.chain[size].data) T(std::forward<U>(in)); // copy or move, can throw
    bucket.chain[size].hash = hashed;                     // emplace hash

    ++bucket.size; // size
    ++counter;     // total size
    return true;
}

template<typename T> auto Hashtable<T>::find(const T& in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Hashtable*>(this)->find(in);
}

template<typename T> auto Hashtable<T>::find(hash_t in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Hashtable*>(this)->find(in);
}

template<typename T> auto Hashtable<T>::at(size_t in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Hashtable*>(this)->at(in);
}

template<typename T> auto Hashtable<T>::begin() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Hashtable*>(this)->begin();
}

template<typename T> auto Hashtable<T>::end() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Hashtable*>(this)->end();
}

} // namespace container
LWE_END
