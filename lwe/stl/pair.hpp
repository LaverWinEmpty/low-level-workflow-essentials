#ifndef LWE_STL_PAIR
#define LWE_STL_PAIR

#include "../meta/meta.h"
#include "../util/hash.hpp"
#include "../container/record.hpp"

LWE_BEGIN
namespace stl {

template<typename Key, typename Value> struct Pair: container::Record<Key, Value>,
                                                    meta::KeyValue {
public:
    using key_type   = Key;   // not first_type
    using value_type = Value; // not second_type

public:
    using container::Record<Key, Value>::Record;

public:
    Key&         first() { return this->key; }
    Value&       second() { return this->value; }
    const Key&   first() const { return this->key; }
    const Value& second() const { return this->value; }

public:
    string serialize() const;
    void   deserialize(const string_view);
};

} // namespace stl

namespace util {
template<typename K, typename V> size_t hashof(const stl::Pair<K, V>& in) {
    return hashof(in.key);
}
} // namespace util

LWE_END
#endif
