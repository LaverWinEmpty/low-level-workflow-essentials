#include "../util/hash.hpp"
#include "hashtable.hpp"

LWE_BEGIN
namespace container {

template<typename K, typename V> void Dictionary<K, V>::push(Entry&& in) {
    emplace(std::move(in));
}

template<typename K, typename V> void Dictionary<K, V>::push(const Entry& in) {
    emplace(in);
}

template<typename K, typename V> void Dictionary<K, V>::push(const K& K, const V& V) {
    emplace(Entry{ K, V });
}

template<typename K, typename V> void Dictionary<K, V>::push(K&& K, V&& V) {
    emplace(Entry{ std::move(K), std::move(V) });
}

template<typename K, typename V> void Dictionary<K, V>::push(const K& K, V&& V) {
    emplace(Entry{ K, std::move(V) });
}

template<typename K, typename V> void Dictionary<K, V>::push(K&& K, const V& V) {
    emplace(Entry{ std::move(K), V });
}

template<typename K, typename V> bool Dictionary<K, V>::pop(const K& in) {
    auto it = find(in);
    if(it != set.end()) {
        set.pop(it);
        return true;
    }
    return false;
}

template<typename K, typename V> bool Dictionary<K, V>::pop(const Iterator<FWD>& in) {
    return set.pop(in);
}

template<typename K, typename V> bool Dictionary<K, V>::pop(hash_t in) {
    return set.pop(in);
}

template<typename K, typename V>
auto Dictionary<K, V>::find(const K& in) noexcept -> Iterator<FWD> {
    // avoiding unnecessary copy logic
    hash_t hashed = util::hashof(in);
    size_t index  = set.indexing(hashed);

    typename Hashtable::Bucket* bucket = set.buckets[index];

    // found
    if(bucket.used) {
        // first data only
        if(bucket->size == 0) {
            typename Hashtable::Iterator it = { &set, index }; // create iterator
            return it;
        }
        // has chain
        for(int i = 0; i < bucket.size; ++i) {
            typename Hashtable::Chain& chain = bucket->chain[i];
            if(hashed == chain.hashed && in == chain.data.first) {
                typename Hashtable::Iterator it = { &set, index, i }; // create iterator
                return it;
            }
        }
    }
    return set.end(); // not found
}

template<typename K, typename V> auto Dictionary<K, V>::find(hash_t in) noexcept -> Iterator<FWD> {
    return set.find(in);
}

template<typename K, typename V> auto Dictionary<K, V>::at(size_t in) noexcept -> Iterator<FWD> {
    return set.at(in);
}

template<typename K, typename V> auto Dictionary<K, V>::begin() noexcept -> Iterator<FWD> {
    return set.begin();
}

template<typename K, typename V> auto Dictionary<K, V>::end() noexcept -> Iterator<FWD> {
    return set.end();
}

template<typename K, typename V>
auto Dictionary<K, V>::find(const K& in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Dictionary*>(this)->find(in);
}

template<typename K, typename V>
auto Dictionary<K, V>::find(hash_t in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Dictionary*>(this)->find(in);
}

template<typename K, typename V>
auto Dictionary<K, V>::at(size_t in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Dictionary*>(this)->at(in);
}

template<typename K, typename V>
auto Dictionary<K, V>::begin() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Dictionary*>(this)->begin();
}

template<typename K, typename V>
auto Dictionary<K, V>::end() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Dictionary*>(this)->end();
}

template<typename K, typename V> size_t Dictionary<K, V>::size() const noexcept {
    return set.counter;
}

template<typename K, typename V> size_t Dictionary<K, V>::capacity() const noexcept {
    return set.capaictor;
}

template<typename K, typename V> bool Dictionary<K, V>::exist(hash_t in) const noexcept {
    return set.exist(in);
}

template<typename K, typename V> bool Dictionary<K, V>::exist(const K& in) const noexcept {
    return find(in) != end();
}

template<typename K, typename V>
auto Dictionary<K, V>::bucket(size_t in) const noexcept -> const typename Hashtable::Bucket* {
    return set.bucket(in);
}

template<typename K, typename V> template<typename U>
bool Dictionary<K, V>::emplace(U&& in) noexcept {
    hash_t hashed = util::hashof(in.first);
    size_t index  = set.indexing(hashed);

    if(set.counter >= set.capacitor) { 
        set.resize(set.log + 1);
    }
    const Hashtable::Bucket& bucket = set.buckets[index]; // get ref

    // duplicated check (pair::first only)
    if(bucket.used == true) {
        if(bucket.data.first == in.first) {
            return false;
        }
        for(uint16_t i = 0; i < bucket.size; ++i) {
            if(bucket.chain[i].data.first == in.first) {
                return false;
            }
        }
    }

    return set.emplace(in, hashed, false); // no check
}

} // namespace container
LWE_END
