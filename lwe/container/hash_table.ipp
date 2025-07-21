#include "../util/hash.hpp"
#include "hashed_buffer.hpp"

LWE_BEGIN
namespace container {

template<typename K, typename V> V& HashTable<K, V>::operator[](const K& in) {
    hash_t hashed = util::Hash<K>(in);

    Chain* pos = slot(hashed, in);
    if(pos == nullptr) {
        // insert and return
        // empty data for call constructor
        if(!push(Entry{ in, V{} })) {
            throw diag::error(diag::BAD_ALLOC);
        }
        return slot(hashed, in)->data.second;
    }
    else return pos->data.second;
}

template<typename K, typename V> const V& HashTable<K, V>::operator[](const K& in) const {
    return const_cast<HashTable*>(this)->operator[](in);
}

template<typename K, typename V> bool HashTable<K, V>::push(Entry&& in) {
    return insert(std::move(in));
}

template<typename K, typename V> bool HashTable<K, V>::push(const Entry& in) {
    return insert(in);
}

template<typename K, typename V> bool HashTable<K, V>::push(const K& K, const V& V) {
    return insert(Entry{ K, V });
}

template<typename K, typename V> bool HashTable<K, V>::push(K&& K, V&& V) {
    return insert(Entry{ std::move(K), std::move(V) });
}

template<typename K, typename V> bool HashTable<K, V>::push(const K& K, V&& V) {
    return insert(Entry{ K, std::move(V) });
}

template<typename K, typename V> bool HashTable<K, V>::push(K&& K, const V& V) {
    return insert(Entry{ std::move(K), V });
}

template<typename K, typename V> bool HashTable<K, V>::pop(const K& in) {
    hash_t  hashed = util::Hash<K>(in);
    Bucket* bucket = slot(hashed);
    Chain*  pos    = slot(hashed, in);

    // not found
    if(pos == nullptr) {
        return false;
    }
    set.remove(bucket, pos);
    return true;
}

template<typename K, typename V> bool HashTable<K, V>::exist(const K& in) const noexcept {
    return slot(util::Hash<K>(in), in) != nullptr;
}

template<typename K, typename V> bool HashTable<K, V>::erase(const Iterator<FWD>& in) {
    if(in != end()) {
        return set.pop(*in);
    }
    return false;
}

template<typename K, typename V>
template<typename T> bool HashTable<K, V>::insert(T&& in) {
    return emplace(std::forward<T>(in));
}

template<typename K, typename V>
template<typename T, typename U> bool HashTable<K, V>::insert(T&& k, U&& v) {
    return emplace(Entry{ std::forward<T>(k), std::forward<U>(v) });
}

template<typename K, typename V>
auto HashTable<K, V>::find(const K& in) noexcept -> Iterator<FWD> {
    if(set.buckets == nullptr) {
        return end();
    }

    // avoiding unnecessary copy logic
    hash_t  hashed = util::Hash<K>(in);
    size_t  index  = set.indexof(hashed);
    Bucket& bucket = set.buckets[index];

    // found
    if(bucket.used) {
        // not chain, or first data
        if(bucket.hash == hashed && bucket.data.first == in) {
            Iterator<FWD> it = { &set, index }; // create iterator
            return it;
        }
        // has chain
        for(uint16_t i = 0; i < bucket.size; ++i) {
            Chain& chain = bucket.chain[i];
            if(hashed == chain.hash && in == chain.data.first) {
                Iterator<FWD> it = { &set, index, i }; // create iterator
                return it;
            }
        }
    }
    return set.end(); // not found
}

template<typename K, typename V> auto HashTable<K, V>::at(size_t in) noexcept -> Iterator<FWD> {
    return set.at(in);
}

template<typename K, typename V> auto HashTable<K, V>::begin() noexcept -> Iterator<FWD> {
    return set.begin();
}

template<typename K, typename V> auto HashTable<K, V>::end() noexcept -> Iterator<FWD> {
    return set.end();
}

template<typename K, typename V>
auto HashTable<K, V>::find(const K& in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<HashTable*>(this)->find(in);
}

template<typename K, typename V>
auto HashTable<K, V>::at(size_t in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<HashTable*>(this)->at(in);
}

template<typename K, typename V>
auto HashTable<K, V>::begin() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<HashTable*>(this)->begin();
}

template<typename K, typename V>
auto HashTable<K, V>::end() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<HashTable*>(this)->end();
}

template<typename K, typename V> size_t HashTable<K, V>::indexof(hash_t in) const noexcept {
    return set.indexof(in);
}

template<typename K, typename V> size_t HashTable<K, V>::size() const noexcept {
    return set.counter;
}

template<typename K, typename V> size_t HashTable<K, V>::capacity() const noexcept {
    return set.capacitor;
}

template<typename K, typename V> bool HashTable<K, V>::reserve(size_t in) noexcept {
    return set.reserve(in);
}

template<typename K, typename V>
auto HashTable<K, V>::bucket(size_t in) const noexcept -> const Bucket* {
    return set.bucket(in);
}

template<typename K, typename V>
auto HashTable<K, V>::slot(hash_t in) noexcept -> Bucket* {
    return set.slot(in);
}

template<typename K, typename V>
auto HashTable<K, V>::slot(hash_t in, const K& data) noexcept -> Chain* {
    if(set.capacitor == 0) {
        set.rehash(set.log); // init
    }
    Bucket* bucket = set.buckets + (set.indexof(in));

    if(bucket->hash == in && bucket->data.first == data) {
        return bucket;
    }
    for(uint16_t i = 0; i < bucket->size; ++i) {
        Chain* chain = bucket->chain + i;
        if(chain->hash == in && chain->data.first == data) {
            return chain;
        }
    }
    return nullptr;
}

template<typename K, typename V>
auto HashTable<K, V>::slot(hash_t in) const noexcept -> const Bucket* {
    return const_cast<HashTable*>(this)->slot(in);
}

template<typename K, typename V>
auto HashTable<K, V>::slot(hash_t in, const K& data) const noexcept -> const Chain* {
    return const_cast<HashTable*>(this)->slot(in, data);
}

template<typename K, typename V>
template<typename T> bool HashTable<K, V>::emplace(T&& in) {
    hash_t hashed = util::Hash<K>(in.first); // first only

    // check collide
    if(slot(hashed, in.first) != nullptr) {
        return false;
    }

    // check size
    if(set.counter >= set.factor) {
        if(!set.rehash(set.log + 1)) {
            return false;
        }
    }
    return set.emplace(in, hashed);
}

} // namespace container
LWE_END
