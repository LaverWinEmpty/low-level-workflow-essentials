#ifndef LWE_CONTAINER_RECORD
#define LWE_CONTAINER_RECORD

#include "../util/hash.hpp"

LWE_BEGIN
namespace container {

template<typename K, typename V> struct Record {
public:
    Record() = default;
    Record(const K&, const V&);
    Record(const K&, V&&);
    Record(K&&, const V&);
    Record(K&&, V&&);
    ~Record() noexcept = default;

public:
    bool operator==(const K&) const; //!< compare: Record to key type
    bool operator!=(const K&) const; //!< compare: Record to key type
    bool operator>(const K&) const;  //!< compare: Record to key type
    bool operator<(const K&) const;  //!< compare: Record to key type
    bool operator>=(const K&) const; //!< compare: Record to key type
    bool operator<=(const K&) const; //!< compare: Record to key type

public:
    operator K() const;          //!< get key
    V&       operator*();        //!< get data
    V*       operator->();       //!< get data
    const V& operator*() const;  //!< get data
    const V* operator->() const; //!< get data

public:
    K key;
    V value;
};
} // namespace container

// hash define
namespace util {
template<typename K, typename V> hash_t hashof(const container::Record<K, V>& in) {
    return hashof<K>(in.key);
}
} // namespace util

LWE_END
#include "record.ipp"
#endif
