#include "../util/hash.hpp"
#include "hashtable.hpp"

LWE_BEGIN
namespace container {

template<typename Key, typename Value> void Dictionary<Key, Value>::push(Entry&& in) {
    set.insert(std::move(in));
}

template<typename Key, typename Value> void Dictionary<Key, Value>::push(const Entry& in) {
    set.insert(in);
}

template<typename Key, typename Value> void Dictionary<Key, Value>::push(const Key& key, const Value& value) {
    set.insert(Entry(key, value));
}

template<typename Key, typename Value> void Dictionary<Key, Value>::push(Key&& key, Value&& value) {
    set.insert(Entry(std::move(key), std::move(value)));
}

template<typename Key, typename Value> void Dictionary<Key, Value>::push(const Key& key, Value&& value) {
    set.insert(Entry(key, std::move(value)));
}

template<typename Key, typename Value> void Dictionary<Key, Value>::push(Key&& key, const Value& value) {
    set.insert(Entry(std::move(key), value));
}

template<typename Key, typename Value> bool Dictionary<Key, Value>::pop(const Key& in) {
    auto it = find(in);
    if(it != set.end()) {
        set.pop(it);
        return true;
    }
    return false;
}

template<typename Key, typename Value> bool Dictionary<Key, Value>::pop(const Iterator<FWD>& in) {
    return set.pop(in);
}

template<typename Key, typename Value> bool Dictionary<Key, Value>::pop(hash_t in) {
    return set.pop(in);
}

template<typename Key, typename Value>
auto Dictionary<Key, Value>::find(const Key& in) noexcept -> Iterator<FWD> {
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
            if(hashed == chain.hashed && in == chain.data.key) {
                typename Hashtable::Iterator it = { &set, index, i }; // create iterator
                return it;
            }
        }
    }
    return set.end(); // not found
}

template<typename Key, typename Value> auto Dictionary<Key, Value>::find(hash_t in) noexcept -> Iterator<FWD> {
    return set.find(in);
}

template<typename Key, typename Value> auto Dictionary<Key, Value>::at(size_t in) noexcept -> Iterator<FWD> {
    return set.at(in);
}

template<typename Key, typename Value> auto Dictionary<Key, Value>::begin() noexcept -> Iterator<FWD> {
    return set.begin();
}

template<typename Key, typename Value> auto Dictionary<Key, Value>::end() noexcept -> Iterator<FWD> {
    return set.end();
}

template<typename Key, typename Value>
auto Dictionary<Key, Value>::find(const Key& in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Dictionary*>(this)->find(in);
}

template<typename Key, typename Value>
auto Dictionary<Key, Value>::find(hash_t in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Dictionary*>(this)->find(in);
}

template<typename Key, typename Value>
auto Dictionary<Key, Value>::at(size_t in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Dictionary*>(this)->at(in);
}

template<typename Key, typename Value>
auto Dictionary<Key, Value>::begin() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Dictionary*>(this)->begin();
}

template<typename Key, typename Value>
auto Dictionary<Key, Value>::end() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Dictionary*>(this)->end();
}

template<typename Key, typename Value> size_t Dictionary<Key, Value>::size() const noexcept {
    return set.size();
}

template<typename Key, typename Value> size_t Dictionary<Key, Value>::capacity() const noexcept {
    return set.capacity();
}

template<typename Key, typename Value> bool Dictionary<Key, Value>::exist(hash_t in) const noexcept {
    return set.exist(in);
}

template<typename Key, typename Value> bool Dictionary<Key, Value>::exist(const Key& in) const noexcept {
    return find(in) != end();
}

template<typename Key, typename Value>
auto Dictionary<Key, Value>::bucket(size_t in) const noexcept -> const typename Hashtable::Bucket* {
    return set.bucket(in);
}

} // namespace container
LWE_END
