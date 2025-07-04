#ifndef LWE_STL_MAP
#define LWE_STL_MAP

#include "set.hpp"
#include "pair.hpp"

LWE_BEGIN
namespace stl {

template<typename Key, typename Value>
class Map {
    // using Set<Pair<Key, Value>> -> registered manually
    using MapElement                 = Pair<Key, Value>;
    using Set                        = Set<MapElement>;
    template<Mod MOD> using Iterator = Iterator<MOD, Set>;

public:
    Value& operator[](const Key& in) {
        Iterator<FWD> it = find(in);
        return it->value;
    }

    const Value& operator[](const Key& in) const {
        Iterator<FWD | VIEW> it = find(in);
        return it->value;
    }


public:
    void push(Pair<Key, Value>&&);
    void push(const Pair<Key, Value>&);
    void push(const Key&, const Value&);
    void push(Key&&, Value&&);
    void push(const Key&, Value&&);
    void push(Key&&, const Value&);

public:
    bool pop(const Key&);
    bool pop(const Iterator<FWD>&);
    bool pop(hash_t);

public:
    Iterator<FWD> find(const Key&) noexcept;
    Iterator<FWD> find(hash_t) noexcept;
    Iterator<FWD> at(size_t) noexcept;
    Iterator<FWD> begin() noexcept;
    Iterator<FWD> end() noexcept;

public:
    Iterator<FWD | VIEW> find(const Key&) const noexcept;
    Iterator<FWD | VIEW> find(hash_t) const noexcept;
    Iterator<FWD | VIEW> at(size_t) const noexcept;
    Iterator<FWD | VIEW> begin() const noexcept;
    Iterator<FWD | VIEW> end() const noexcept;

public:
    size_t size() const noexcept;
    size_t capacity() const noexcept;
    bool   exist(hash_t) const noexcept;
    bool   exist(const Key&) const noexcept;

public:
    const typename Set::Bucket* bucket(size_t in) const noexcept;

public:
    Set set;
};

} // namespace stl
LWE_END
#include "map.ipp"
#endif
