#ifndef LWE_STL_SET
#define LWE_STL_SET

/**************************************************************************************************
 * HASH SET with enhanced chaining (inline first element + array-based chains)
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
#include "../config/config.h"
#include "../util/hash.hpp"

LWE_BEGIN
namespace stl {
template<typename T> class Set: public meta::Container {
    CONTAINER_BODY(T, Set, T);
    template<typename, typename> friend class Map;

    struct Chain {
        T      data; // data
        hash_t hash; // calculated hash
    };

    struct Bucket: Chain {
        using Chain::data;
        using Chain::hash;

        Chain*   chain;    // used when hash collide
        uint16_t size;     // chain size (max 65535)
        uint16_t capacity; // chain capaicty (max 65535)
        bool     used;     // false == empty
    };

public:
    using Grower = uint16_t (*)(uint16_t);

public:
    //! @brief constructor
    //! @param [in] factor: load factor
    //! @param [in] grower: chain array grow strategy
    Set(float factor = config::LOADFACTOR, Grower grower = [](uint16_t in) { return uint16_t(in + 1); });

public:
    //! @brief constructor, load factor is default
    //! @param [in] grower: chain array grow strategy
    Set(Grower grower);

public:
    //! @brief free
    ~Set();

public:
    bool push(T&&);
    bool push(const T&);
    bool pop(const T&) noexcept;
    bool pop(const Iterator<FWD>&) noexcept;
    bool pop(hash_t) noexcept;
    bool exist(const T&) noexcept;
    bool exist(hash_t) noexcept;
    bool reserve(size_t size) noexcept;
    void clear() noexcept;

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
    Grower  grower;

public:
    const float LOAD_FACTOR;
};

REGISTER_CONTAINER(Set, STL_SET);

} // namespace stl
LWE_END
#include "set.ipp"
#endif
