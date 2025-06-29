LWE_BEGIN
namespace stl {

template<typename Key, typename Value> Pair<Key, Value>::Pair(const Key& key, const Value& value):
    key(key),
    value(value) { }

template<typename Key, typename Value> Pair<Key, Value>::Pair(const Key& key, Value&& value):
    key(key),
    value(std::move(value)) { }

template<typename Key, typename Value> Pair<Key, Value>::Pair(Key&& key, const Value& value):
    key(std::move(key)),
    value(value) { }

template<typename Key, typename Value> Pair<Key, Value>::Pair(Key&& key, Value&& value):
    key(std::move(key)),
    value(std::move(value)) { }

template<typename Key, typename Value> bool Pair<Key, Value>::operator==(const Pair& in) const {
    return key == in.key;
}

template<typename Key, typename Value> bool Pair<Key, Value>::operator!=(const Pair& in) const {
    return key != in.key;
}

template<typename Key, typename Value> bool Pair<Key, Value>::operator<(const Pair& in) const {
    return key < in.key;
}

template<typename Key, typename Value> bool Pair<Key, Value>::operator>(const Pair& in) const {
    return key > in.key;
}

template<typename Key, typename Value> bool Pair<Key, Value>::operator<=(const Pair& in) const {
    return key <= in.key;
}

template<typename Key, typename Value> bool Pair<Key, Value>::operator>=(const Pair& in) const {
    return key >= in.key;
}

template<typename Key, typename Value> Pair<Key, Value>::operator hash_t() const {
    return LWE::util::hashof<Key>(key);
}

template<typename Key, typename Value> Key& Pair<Key, Value>::first() {
    return key;
}

template<typename Key, typename Value> Value& Pair<Key, Value>::second() {
    return value;
}

template<typename Key, typename Value> const Key& Pair<Key, Value>::first() const {
    return key;
}

template<typename Key, typename Value> const Value& Pair<Key, Value>::second() const {
    return value;
}

} // namespace stl
LWE_END
