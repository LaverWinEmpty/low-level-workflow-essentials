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
        size_t max = self->capacitor;
        if(index == max) {
            return *this; // end
        }
        // chaining
        if(chaining == false) {
            chaining = true;
        }
        else ++chain;
        if(chain >= self->buckets[index].capacity) {
            chaining = false;
            chain = 0;
            // next
            while(index < max) {
                ++index;
                if(self->buckets[index].used == true) {
                    break; // found
                }
            }
        }
        return *this;
    }

    Iterator& operator--() {
        if(index == 0 && chaining == false) {
            return *this; // end
        }
        // chaining and prev
        if(chaining == true) {
            if(chain > 0) {
                --chain;
            }
            else chaining = false;
        }
        // find prev bucket
        else {
            while(index > 0) {
                --index;
                if(self->buckets[index].used == true) {
                    if(self->buckets[index].size != 0) {
                        chaining = true;                          // chain
                        chain    = self->buckets[index].size - 1; // chain end
                    }
                    break;
                }
            }
        }
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
        std::free(buckets);
    }
}

template<typename T> bool Hashtable<T>::push(T&& in) {
    return insert(std::move(in));
}

template<typename T> bool Hashtable<T>::push(const T& in) {
    return insert(in);
}

template<typename T> bool Hashtable<T>::pop(const T& in) noexcept {
    hash_t  hashed = util::Hash<T>(in);          // get hash
    Bucket* bucket = buckets + indexof(hashed); // get bucket
    Chain*  pos    = slot(hashed, in);          // get delete pos
    if(pos == nullptr) {
        return false; // not found
    }
    remove(bucket, pos);
    return true;
}

template<typename T> bool Hashtable<T>::exist(const T& in) noexcept {
    return slot(in, util::Hash<T>(in)) != nullptr;
}

template<typename T>
template<typename U> bool Hashtable<T>::insert(U&& in) {
    hash_t hashed = util::Hash<T>(in);
    // check
    if(slot(hashed, in) != nullptr) {
        return false;
    }
    if(counter >= factor) {
        // capacitor << 1
        if(!rehash(log + 1)) {
            return false;
        }
    }
    return emplace(std::forward<U>(in), hashed);
}

template<typename T> bool Hashtable<T>::erase(const Iterator<FWD>& in) noexcept {
    Bucket* bucket = buckets[in.index]; // get bucket
    if(in.self != this || in.chain >= bucket->capacity) {
        return false; // exception
    }
    Chain* chain = in.chaining ? (bucket + in.chain) : bucket; // get data
    remove(bucket, chain);
    return true;
}

template<typename T> size_t Hashtable<T>::indexof(hash_t in) const noexcept {
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

    // rehash
    if(!rehash(inlog)) {
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
    if(buckets == nullptr) { 
        return end();
    }

    hash_t  hashed = util::Hash<T>(in); // get hash
    size_t  index  = indexof(hashed);
    Bucket& bucket = buckets[index];

    // check has data
    if(bucket.used == true) {
        // not chain, or first data
        if(bucket.hash == hashed && bucket.data == in) {
            return Iterator<FWD>(this, index);
        }
        // return chain
        for(uint16_t i = 0; i < bucket.size; ++i) {
            Chain* chain = bucket.chain + i;
            if(chain->hash == hashed && chain->data == in) {
                return Iterator<FWD>(this, index, i);
            }
        }
    }
    return end(); // not found
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

template<typename T> auto Hashtable<T>::find(const T& in) const noexcept -> Iterator<FWD | VIEW> {
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

template<typename T>
template<typename U> bool Hashtable<T>::emplace(U&& in, hash_t hashed) {
    Bucket* bucket = buckets + (indexof(hashed));
    Chain*  pos    = nullptr;
    if(!bucket) throw diag::error(diag::INVALID_DATA);

    // false -> empty
    if(bucket->used == false) pos = bucket; // first
    else {
        if(bucket->size >= bucket->capacity && !expand(bucket)) {
            return false; // no space
        }
        pos = bucket->chain + bucket->size; // get last
    }

    // copy or move, can throw
    new(&pos->data) T(std::forward<U>(in));

    pos->hash = hashed; // store
    if(bucket == pos) {
        bucket->used = true; // first
    }
    else bucket->size += 1; // chain
    ++counter;
    return true;
}


template<typename T> void Hashtable<T>::remove(Bucket* bucket, Chain* del) {
    // delete first data
    if(bucket == del) {
        // has chain, swap and delete
        if(bucket->size != 0) {
            bucket->size -= 1;                            // reduce
            Chain* last   = bucket->chain + bucket->size; // get last
            del->data     = std::move(last->data);        // move (override)
            del->hash     = last->hash;                   // copy hash
            last->data.~T();                              // delete
        }
        // delete
        else {
            bucket->used = false; // mark
            bucket->data.~T();    // delete
        }
    }

    // else delete chain
    else {
        --bucket->size; // count
        // swap and delete
        if(bucket->size != 0) {
            Chain* last = bucket->chain + bucket->size; // get last
            del->data   = std::move(last->data);        // move (override)
            del->hash   = last->hash;                   // copy hash
            last->data.~T();                            // delete
        }
        else del->data.~T(); // delete
    }
    --counter; // total count
}

template<typename T> bool Hashtable<T>::rehash(uint64_t caplog) {
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
    size_t loop = capacitor; // old size for loop

    log       = caplog;                     // update for fibonachi hash
    capacitor = size;                       // update
    factor    = size_t(size * LOAD_FACTOR); // update
    counter   = 0;                          // reser for recount

    // has data
    if(old) {
        // move
        for(size_t i = 0; i < loop; ++i) {
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
    return true;
}

template<typename T> bool Hashtable<T>::expand(Bucket* in) {
    uint16_t cap   = grower(in->capacity);
    Chain*   newly = static_cast<Chain*>(std::malloc(sizeof(Chain) * cap));
    if(!newly) {
        return false; // bad alloc
    }

    // remove old and move
    Chain* old = in->chain;
    for(uint16_t i = 0; i < in->size; ++i) {
        new(&newly[i].data) T(std::move(old[i].data)); // move, maybe noexcept
        newly[i].hash = old[i].hash;                   // copy hash
        old[i].data.~T();                              // dtor
    }
    in->chain    = newly; // store
    in->capacity = cap;   // count
    std::free(old);       // delete
    return true;
}

template<typename T> auto Hashtable<T>::bucket(size_t in) const noexcept -> const Bucket* { 
    if(in >= capacitor) {
        return nullptr; // exception
    }
    return buckets + in;
}

template<typename T> auto Hashtable<T>::slot(hash_t in) noexcept -> Bucket* {
    if (capacitor == 0) {
        rehash(log); // init
    }
    return bucket + indexof(in);
}

template<typename T> auto Hashtable<T>::slot(hash_t in, const T& data) noexcept -> Chain* {
    if(capacitor == 0) {
        rehash(log); // init
    }
    Bucket* bucket = buckets + (indexof(in));

    if(bucket->hash == in && bucket->data == data) {
        return bucket;
    }
    for(uint16_t i = 0; i < bucket->size; ++i) {
        Chain* chain = bucket->chain + i;
        if(chain->hash == in && chain->data == data) {
            return chain;
        }
    }
    return nullptr;
}

template<typename T> auto Hashtable<T>::slot(hash_t in) const noexcept -> const Bucket* {
    return const_cast<Hashtable*>(this)->slot(in);
}

template<typename T> auto Hashtable<T>::slot(hash_t in, const T& data) const noexcept -> const Chain* {
    return const_cast<Hashtable*>(this)->slot(in, data);
}

} // namespace container
LWE_END
