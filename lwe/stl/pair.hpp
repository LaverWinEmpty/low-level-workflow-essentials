#ifndef LWE_STL_PAIR
#define LWE_STL_PAIR

#include "../base/base.h"
#include "../meta/internal/feature.hpp"

LWE_BEGIN

namespace stl {

template<typename Key, typename Value> struct Pair: meta::Pair {
    using KeyType   = Key;
    using ValueType = Value;

public:
    Pair() = default;
    Pair(const Key&, const Value&);
    Pair(const Key&, Value&&);
    Pair(Key&&, const Value&);
    Pair(Key&&, Value&&);
    ~Pair() noexcept = default;

public:
    bool operator==(const Pair&) const;
    bool operator!=(const Pair&) const;
    bool operator>(const Pair&) const;
    bool operator<(const Pair&) const;
    bool operator>=(const Pair&) const;
    bool operator<=(const Pair&) const;

public:
    bool operator==(const Key&) const;
    bool operator!=(const Key&) const;
    bool operator>(const Key&) const;
    bool operator<(const Key&) const;
    bool operator>=(const Key&) const;
    bool operator<=(const Key&) const;

public:
    Value&       operator*();
    Value*       operator->();
    const Value& operator*() const;
    const Value* operator->() const;

public:
    operator Key() const; // to hash

public:
    Key&         first();
    Value&       second();
    const Key&   first() const;
    const Value& second() const;

public:
    string serialize() const;
    void   deserialize(const string_view);

public:
    Key   key;
    Value value;
};

} // namespace stl
LWE_END
#include "pair.ipp"
#endif
