LWE_BEGIN
namespace stl {

template<typename Key, typename Value> void Map<Key, Value>::push(Pair<Key, Value>&& in) {
    set.insert(std::move(in));
}

template<typename Key, typename Value> void Map<Key, Value>::push(const Pair<Key, Value>& in) {
    set.insert(in);
}

template<typename Key, typename Value> void Map<Key, Value>::push(const Key& key, const Value& value) {
    set.insert(Pair<Key, Value>(key, value));
}

template<typename Key, typename Value> void Map<Key, Value>::push(Key&& key, Value&& value) {
    set.insert(Pair<Key, Value>(std::move(key), std::move(value)));
}

template<typename Key, typename Value> void Map<Key, Value>::push(const Key& key, Value&& value) {
    set.insert(Pair<Key, Value>(key, std::move(value)));
}

template<typename Key, typename Value> void Map<Key, Value>::push(Key&& key, const Value& value) {
    set.insert(Pair<Key, Value>(std::move(key), value));
}

template<typename Key, typename Value> bool Map<Key, Value>::pop(const Key& in) {
    auto it = find(in);
    if(it != set.end()) {
        set.pop(it);
        return true;
    }
    return false;
}

template<typename Key, typename Value> bool Map<Key, Value>::pop(const Iterator<FWD>& in) {
    return set.pop(in);
}

template<typename Key, typename Value> bool Map<Key, Value>::pop(hash_t in) {
    return set.pop(in);
}

template<typename Key, typename Value>
auto Map<Key, Value>::find(const Key& in) noexcept -> Iterator<FWD> {
    // avoiding unnecessary copy logic
    hash_t hashed = util::hashof(in);
    size_t index  = set.indexing(hashed);

    typename Set::Bucket* bucket = set.buckets[index];

    // found
    if(bucket.used) {
        // first data only
        if(bucket->size == 0) {
            typename Set::Iterator it = { &set, index }; // create iterator
            return it;
        }
        // has chain
        for(int i = 0; i < bucket.size; ++i) {
            typename Set::Chain& chain = bucket->chain[i];
            if(hashed == chain.hashed && in == chain.data.key) {
                typename Set::Iterator it = { &set, index, i }; // create iterator
                return it;
            }
        }
    }
    return set.end(); // not found
}

template<typename Key, typename Value> auto Map<Key, Value>::find(hash_t in) noexcept -> Iterator<FWD> {
    return set.find(in);
}

template<typename Key, typename Value> auto Map<Key, Value>::at(size_t in) noexcept -> Iterator<FWD> {
    return set.at(in);
}

template<typename Key, typename Value> auto Map<Key, Value>::begin() noexcept -> Iterator<FWD> {
    return set.begin();
}

template<typename Key, typename Value> auto Map<Key, Value>::end() noexcept -> Iterator<FWD> {
    return set.end();
}

template<typename Key, typename Value>
auto Map<Key, Value>::find(const Key& in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Map*>(this)->find(in);
}

template<typename Key, typename Value>
auto Map<Key, Value>::find(hash_t in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Map*>(this)->find(in);
}

template<typename Key, typename Value>
auto Map<Key, Value>::at(size_t in) const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Map*>(this)->at(in);
}

template<typename Key, typename Value>
auto Map<Key, Value>::begin() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Map*>(this)->begin();
}

template<typename Key, typename Value>
auto Map<Key, Value>::end() const noexcept -> Iterator<FWD | VIEW> {
    return const_cast<Map*>(this)->end();
}

template<typename Key, typename Value> size_t Map<Key, Value>::size() const noexcept {
    return set.size();
}

template<typename Key, typename Value> size_t Map<Key, Value>::capacity() const noexcept {
    return set.capacity();
}

template<typename Key, typename Value> bool Map<Key, Value>::exist(hash_t in) const noexcept {
    return set.exist(in);
}

template<typename Key, typename Value> bool Map<Key, Value>::exist(const Key& in) const noexcept {
    return find(in) != end();
}

template<typename Key, typename Value>
auto Map<Key, Value>::bucket(size_t in) const noexcept -> const typename Set::Bucket* {
    return set.bucket(in);
}

} // namespace stl
LWE_END
