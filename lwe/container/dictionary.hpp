#ifndef LWE_CONTAINER_DICTIONARY
#define LWE_CONTAINER_DICTIONARY

#include "../config/config.h"
#include "record.hpp"
#include "iterator.hpp"

LWE_BEGIN
namespace container {

template<typename> class Hashtable;

template<typename K, typename V> class Dictionary {
public:
    using Entry     = Record<K, V>;
    using Hashtable = Hashtable<Entry>;
    template<Mod, typename> friend class Iterator; //!< iterator
    template<Mod MOD> using Iterator = Iterator<MOD, Hashtable>;

public:
    using value_type             = Entry;
    using iterator               = Iterator<FWD>;
    using reverse_iterator       = Iterator<BWD>;
    using const_iterator         = Iterator<FWD | VIEW>;
    using const_reverse_iterator = Iterator<BWD | VIEW>;

public:
    V& operator[](const K& in) {
        Iterator<FWD> it = find(in);
        return it->value;
    }

    const V& operator[](const K& in) const {
        Iterator<FWD | VIEW> it = find(in);
        return it->value;
    }

public:
    void push(Entry&&);
    void push(const Entry&);
    void push(const K&, const V&);
    void push(K&&, V&&);
    void push(const K&, V&&);
    void push(K&&, const V&);

public:
    bool pop(const K&);
    bool pop(const Iterator<FWD>&);
    bool pop(hash_t);

public:
    Iterator<FWD> find(const K&) noexcept;
    Iterator<FWD> find(hash_t) noexcept;
    Iterator<FWD> at(size_t) noexcept;
    Iterator<FWD> begin() noexcept;
    Iterator<FWD> end() noexcept;

public:
    Iterator<FWD | VIEW> find(const K&) const noexcept;
    Iterator<FWD | VIEW> find(hash_t) const noexcept;
    Iterator<FWD | VIEW> at(size_t) const noexcept;
    Iterator<FWD | VIEW> begin() const noexcept;
    Iterator<FWD | VIEW> end() const noexcept;

public:
    size_t size() const noexcept;
    size_t capacity() const noexcept;
    bool   exist(hash_t) const noexcept;
    bool   exist(const K&) const noexcept;

public:
    const typename Hashtable::Bucket* bucket(size_t in) const noexcept;

public:
    Hashtable set;
};

} // namespace container
LWE_END
#include "dictionary.ipp"
#endif
