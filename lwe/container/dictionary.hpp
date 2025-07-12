#ifndef LWE_CONTAINER_DICTIONARY
#define LWE_CONTAINER_DICTIONARY

#include "../config/config.h"
#include "iterator.hpp"

LWE_BEGIN
namespace container {

template<typename> class Hashtable;

template<typename K, typename V>
struct Record: public std::pair<K, V> {
    using std::pair<K, V>::pair;

    bool operator==(const std::pair<K, V>& in) const { return this->first == in.first; }
    bool operator!=(const std::pair<K, V>& in) const { return this->first != in.first; }
};

template<typename K, typename V> class Dictionary {
public:
    using Entry = Record<K, V>;
public:
    CONTAINER_BODY(Hashtable, Entry, Entry);
private:
    using Hashtable = Hashtable<Entry>;
    using Bucket    = typename Hashtable::Bucket;
    using Chain     = typename Hashtable::Chain;

public:
    V&       operator[](const K&);
    const V& operator[](const K& in) const;

public:
    bool push(Entry&&);
    bool push(const Entry&);
    bool push(const K&, const V&);
    bool push(K&&, V&&);
    bool push(const K&, V&&);
    bool push(K&&, const V&);
    bool pop(const K&);
    bool exist(const K&) const noexcept;

public:
    template<typename T> bool             insert(T&&);
    template<typename T, typename U> bool insert(T&&, U&&);
    bool                                  erase(const Iterator<FWD>&);

public:
    Iterator<FWD> find(const K&) noexcept;
    Iterator<FWD> at(size_t) noexcept;
    Iterator<FWD> begin() noexcept;
    Iterator<FWD> end() noexcept;

public:
    Iterator<FWD | VIEW> find(const K&) const noexcept;
    Iterator<FWD | VIEW> at(size_t) const noexcept;
    Iterator<FWD | VIEW> begin() const noexcept;
    Iterator<FWD | VIEW> end() const noexcept;

public:
    size_t indexof(hash_t) const noexcept;
    size_t size() const noexcept;
    size_t capacity() const noexcept;
    bool   reserve(size_t) noexcept;

public:
    const Bucket* bucket(size_t) const noexcept; //!< get bucket(index)

private:
    Bucket*       slot(hash_t) noexcept;                 //!< get bucket slot
    Chain*        slot(hash_t, const K&) noexcept;       //!< get chain slot
    const Bucket* slot(hash_t) const noexcept;           //!< get bucket slot
    const Chain*  slot(hash_t, const K&) const noexcept; //!< get chain slot

private:
    template<typename T> bool emplace(T&&);

public:
    Hashtable set;
};

} // namespace container
LWE_END
#include "dictionary.ipp"
#endif
