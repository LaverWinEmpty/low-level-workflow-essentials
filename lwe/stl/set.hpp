#ifndef LWE_STL_SET
#define LWE_STL_SET

/**************************************************************************************************
 * HASH SET that open addressing + chaining bybrid
 *
 * hashing
 * - integer types    -> used as-is
 * - floating types   -> cast byte-wise to integer
 * - long double type -> if 8 byte as double; else as string
 * - other types      -> FNV-1a
 * - indexing         -> Fibonacci hashing (replaces modulo)
 *
 * memory rayout
 *   [0]->+-------+
 *        | data  |   +------+------+
 *        | chain --->| data | data | ... < collision chain (array)
 *   [1]->+-------+   +------+------+
 *        | data  |
 *        | chain ---> nullptr < no collision
 *        +-------+
 *        |  ...  |
 * [2^k]->+-------+
 *        | data  |
 *        | chain ---> ...
 * [end]->+-------+
 *            ^
 *            bucket is array, capacity is power of 2
 *
 * state
 *  bucket.used == false -> empty
 *  bucket.used == true  -> exist
 *  bucket.size == 0     -> no chain
 *  bucket.size >= 0     -> chaining
 **************************************************************************************************/

#include "../meta/meta.h"
#include "../util/hash.hpp"

LWE_BEGIN
namespace stl {
template<typename T> class Set: public meta::Container {
    CONTAINER_BODY(T, Set, T);
    static constexpr float LOAD_FACTOR = 0.75f; // TODO: move to config

    template<typename, typename> friend class Map;

    struct Chain {
        T      data; // data
        hash_t hash; // calculated hash
    };

    struct Bucket: Chain {
        using Chain::data;
        using Chain::hash;

        Chain*  chain;    // used when hash collide
        uint8_t size;     // chain size (max 255)
        uint8_t capacity; // chain capaicty (max 255)
        bool    used;     // false == empty
    };

public:
    ~Set();

public:
    bool push(T&&);
    bool push(const T&);
    bool pop(const T&);
    bool pop(const Iterator<FWD>&);
    bool pop(hash_t);
    bool exist(const T&);
    bool exist(hash_t);

public:
    Iterator<FWD | VIEW> find(const T&) const noexcept; //!< find by data
    Iterator<FWD | VIEW> find(hash_t) const noexcept;   //!< find by hash
    Iterator<FWD | VIEW> at(size_t) const noexcept;     //!< find by order
    Iterator<FWD | VIEW> begin() const noexcept;        //!< get begin
    Iterator<FWD | VIEW> end() const noexcept;          //!< get end

public:
    Iterator<FWD> find(const T&) noexcept; //!< non-const
    Iterator<FWD> find(hash_t) noexcept;   //!< non-const
    Iterator<FWD> at(size_t) noexcept;     //!< non-const
    Iterator<FWD> begin() noexcept;        //!< non-const
    Iterator<FWD> end() noexcept;          //!< non-const

private:
    template<typename U> bool insert(U&&);
    template<typename U> bool insert(U&&, hash_t);

private:
    bool resize();

public:
    size_t indexing(hash_t) const noexcept;

public:
    size_t        size() const noexcept;
    size_t        capacity() const noexcept;
    const Bucket* bucket(size_t) const noexcept; // get bucket

private:
    size_t counter   = 0; //!< element counter
    size_t capacitor = 0; //!< bucket counter
    size_t factor    = 0; //!< load factor
    size_t log       = 3; //!< log2(capacitor): default (1 << 3) == 8

private:
    Bucket* buckets = nullptr;
};

REGISTER_CONTAINER(Set, STL_SET);

} // namespace stl
LWE_END
#include "set.ipp"
#endif
