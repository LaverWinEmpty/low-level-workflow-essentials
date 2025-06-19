#include <stdexcept>

// temp
using uint = unsigned int;
using hash_t  = size_t;
using index_t = int;
using uint32_t = unsigned long;
using uint8_t = unsigned char;

// temp
namespace util {
    template<typename T> hash_t hashof(const T&) { return 0; }
}

template<typename T> class Set {
    static constexpr size_t  INITIALIZE  = 8;
    static constexpr float   LOAD_FACTOR = 0.75f;

    struct Chain {
        T      data; // data
        hash_t hash; // store
    };

    struct Bucket : Chain {
        using Chain::data;
        using Chain::hash;

        Chain*  chain;    // used when hash collide
        uint8_t size;     // chain size (max 255)
        uint8_t capacity; // chain capaicty (max 255)
        bool    used;     // false == empty
    };

public:
    struct Iterator {
        Iterator(Set* self, size_t index):
            self(self), index(index), chain(0), chaining(false), end(false) {}

    public:
        Iterator& operator++() {
            if(end) {
                return *this;
            }

            // copy
            index_t index    = this->index;
            uint8_t chain    = this->chain;
            bool    chaining = this->chaining;

            // check
            bool valid = false;

            while (index != self->capacitor) {
                if (flag == false) {
                    flag = true; // main -> chain

                    // chain size == 0 -> no chains, continue
                    if (self->buckets[index].size != 0) {
                        valid = true;
                        break;
                    }
                }
                else {
                    ++chain; // next
                    // end chain
                    if(chain == self->buckets[index].size) {
                        chain = 0;    // init
                        flag = false; // chain -> main
                        ++index;      // next

                        // used == false -> empty, continue
                        if (self->buckets[index].used == true) {
                            valid = true;
                            break;
                        }
                    }
                    // in chain
                    else {
                        valid = true;
                        break;
                    }
                }
            } // end while

            if(valid == true) {
                // set result
                this->index    = index;
                this->chain    = chain;
                this->chaining = chaining;
            }
            else end = true; // invalud -> check end
            return *this;
        }

    public:
        T& operator*() {
            Bucket& bucket = self->buckets[index];
            if(chain >= bucket.size) {
                return bucket.chain[bucket.size - 1].data; // dangling
            }
            return bucket.chain[chain].data; // return
        }

    private:
        Set*    self;
        index_t index;
        uint8_t chain;
        bool    chaining; // false == index, true == chain
        bool    end;
    };

public:
    ~Set();

private:
    bool resize();

public:
    bool push(T&&);
    bool push(const T&);
    
public:
    bool pop(const T&);

public:
    T& at(size_t);

private:
    template<typename U> bool insert(U&&);
    template<typename U> bool insert(U&&, hash_t);

public:
    size_t counter   = 0; //!< counter
    size_t capacitor = 0; //!< container capacitor
    size_t factor    = 0; //!< load factor

private:
    Bucket* buckets = nullptr;
};

template<typename T> Set<T>::~Set() {
    for (size_t i = 0; i < capacitor; ++i) {
        Bucket& bucket = buckets[i];
        if (bucket.used == false) {
            continue; // ignore
        }
        Chain* chain = bucket.chain;
        for(uint8_t j = 0; j < bucket.size; ++j) {
            chain[j].data.~T(); // dtor
        }
        bucket.data.~T(); // dtor
        std::free(chain); // delete
    }
    std::free(buckets);
}

template<typename T> bool Set<T>::resize() {
    size_t size = 0;
    if (capacitor) {
        size = capacitor << 1; // x2
        if (size < capacitor) {
            return false; // overflow
        }
    }
    else size = INITIALIZE; // init

    // realloc
    Bucket* old = buckets;
    buckets = static_cast<Bucket*>(std::malloc(sizeof(Bucket) * size));
    if (!buckets) {
        buckets = old; // back
        return false;  // bad alloc
    }

    // initialize
    for (int i = 0; i < size; ++i) {
        buckets[i].used     = false;
        buckets[i].chain    = nullptr;
        buckets[i].size     = 0;
        buckets[i].capacity = 0;
    }

    // move
    for (size_t i = 0; i < capacitor; ++i) {
        // main data
        if(old[i].used == true) {
            insert(std::move(old[i]), old[i].hash);
        }
        // chained datas
        if (old[i].chain != nullptr) {
            for(uint8_t i = 0; i < old[i].size; ++i) {
                Chain& chain = old[i].chain[i];
                insert(std::move(chain.data), chain.hash); // move
                chain.data.~T();                           // delete
            }
            std::free(old[i].chain); // delete
        }
    }

    return true;
}

template<typename T> bool Set<T>::push(T&& in) {
    return insert(std::move(in));
}

template<typename T> bool Set<T>::push(const T& in) {
    return insert(in);
}

template<typename T> bool Set<T>::pop(const T& in) {
    hash_t  hashed = util::hashof<T>(in);      // get hash
    index_t index  = hashed & (capacitor - 1); // get index

    Bucket& bucket = buckets[index];

    // check first data: perform hash comparison first
    if(bucket.hash == hashed && bucket.data == in) {
        // has chain, swap and deled
        if (bucket.size != 0) {
            bucket.size -= 1;                      // reduce
            Chain& last  = bucket.chain[size - 1]; // get last
            bucket.data  = std::move(last.data);   // move
            chain.data.~T();                       // delete
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
        if (chain.hash == hashed && chain.data == in) {
            --bucket.size; // count
            --counter;     // total count

            // swap and delete
            if (bucket.size != 0) {
                Chain& last = bucket.chain[size - 1]; // get last
                chain.data = std::move(last.data);    // move
                chain.hash = last.hash;               // copy hash
                last.data.~T();                       // delete
            }
            else chain.data.~T(); // delete
            return true;          // succeeded
        }
    }
    return false; // not found
}

template<typename T> T& Set<T>::at(size_t index) {
    size_t pass = 0;
    // out of range
    if (index >= counter) {
        throw std::out_of_range();
    }

    // loop
    for(size_t i = 0; i < capacitor; ++i) {
        // empty bucket pass
        if (buckets[i].used == false) {
            continue;
        }
        // check bucket pos
        if(pass == index) {
            return buckets[i].data;
        }
        pass += 1; // bucket pass

        // check chain
        if (buckets[i].size == 0) {
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
        if (index < (pass + size)) {
            return buckets[index - pass]; 
        }
        pass += size; // chain pass
    } // end for

    // not entered
    char dummy[sizeof(T)];
    return reinterpret_cast<T*>(dummy);
}

template<typename T>
template<typename U>
bool Set<T>::insert(U&& in) {
    // size check
    if(counter >= factor) {
        if(!resize()) {
            return false; // bad alloc
        }
        factor = capacitor * LOAD_FACTOR;
    }
    hash_t hash = util::hashof<T>(in); // const T&
    return insert(std::forward<U>(in), hash);
}

template<typename T>
template<typename U>
bool Set<T>::insert(U&& in, hash_t hashed) {
    index_t index = hashed & (capacitor - 1); // hash % size

    Bucket& bucket = buckets[index];

    // duplicated check
    if(bucket.data == in) {
        return false;
    }
    for(uint8_t i = 0; i < bucket.size; ++i) {
        if(bucket.chain[i].data == in) {
            return false;
        }
    }

    // false -> empty
    if(bucket.used == false) {
        bucket.used = true;                       // check
        bucket.hash = hashed;                     // store
        new(&bucket.data) T(std::forward<U>(in)); // insert
        return true;
    }

    // true -> use chain
    uint8_t size = bucket.size;     // get size
    uint8_t cap  = bucket.capacity; // get capacity

    // check
    if(size >= cap) {
        // alloc chain and check nullptr
        Chain* newly = static_cast<Chain*>(std::malloc((cap + 1) * sizeof(Chain)));
        if (!newly) {
            return false; // bad alloc
        }

        // remove old and move
        Chain* old = bucket.chain;
        for (uint8_t i = 0; i < size; ++i) {
            new (&newly[i].data) T(std::move(old[i].data)); // move
            newly[i].hash = old[i].hash;                    // copy hash
            old[i].data.~T();                               // dtor
        }
        bucket.chain     = newly; // store
        bucket.capacity += 1;     // count
        std::free(old);           // delete
    }

    new (&bucket.chain[size].data) T(std::forward<U>(in)); // isnert data
    bucket.chain[size].hash = hashed;                      // insert hash

    ++bucket.size; // size
    ++counter;      // total size
    return true;
}