LWE_BEGIN
namespace container {

template<typename K, typename V> Record<K, V>::Record(const K& key, const V& value): key(key), value(value) { }

template<typename K, typename V> Record<K, V>::Record(const K& key, V&& value): key(key), value(std::move(value)) { }

template<typename K, typename V> Record<K, V>::Record(K&& key, const V& value): key(std::move(key)), value(value) { }

template<typename K, typename V> Record<K, V>::Record(K&& key, V&& value):
    key(std::move(key)),
    value(std::move(value)) { }

template<typename K, typename V> bool Record<K, V>::operator==(const K& in) const {
    return key == in;
}

template<typename K, typename V> bool Record<K, V>::operator!=(const K& in) const {
    return key != in;
}

template<typename K, typename V> bool Record<K, V>::operator<(const K& in) const {
    return key < in;
}

template<typename K, typename V> bool Record<K, V>::operator>(const K& in) const {
    return key > in;
}

template<typename K, typename V> bool Record<K, V>::operator<=(const K& in) const {
    return key <= in;
}

template<typename K, typename V> bool Record<K, V>::operator>=(const K& in) const {
    return key >= in;
}

template<typename K, typename V> V& Record<K, V>::operator*() {
    return value;
}

template<typename K, typename V> V* Record<K, V>::operator->() {
    return &value;
}

template<typename K, typename V> const V& Record<K, V>::operator*() const {
    return value;
}

template<typename K, typename V> const V* Record<K, V>::operator->() const {
    return &value;
}

template<typename K, typename V> Record<K, V>::operator K() const {
    return key;
}

} // namespace container
LWE_END
