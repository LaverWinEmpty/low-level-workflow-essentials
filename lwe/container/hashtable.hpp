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

#ifndef LWE_CONTAINER_HASHTABLE
#define LWE_CONTAINER_HASHTABLE

#include "../base/base.h"
#include "../config/config.h"
#include "../util/hash.hpp"
#include "iterator.hpp"

LWE_BEGIN
namespace container {

template<typename T> class Hashtable {
public:
    template<typename, typename> friend class Dictionary; //!< for composition

public:
    CONTAINER_BODY(Hashtable, T, T);

private:
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
    Hashtable(float factor = config::LOADFACTOR, Grower grower = [](uint16_t in) { return uint16_t(in + 1); });

public:
    //! @brief constructor, load factor is default
    //! @param [in] grower: chain array grow strategy
    Hashtable(Grower grower);

public:
    //! @brief free
    ~Hashtable();

public:
    bool push(T&&);                //!< push
    bool push(const T&);           //!< push
    bool pop(const T&) noexcept;   //!< pop
    bool exist(const T&) noexcept; //!< check data

public:
    template<typename U> bool insert(U&&);                          //!< insert
    bool                      erase(const Iterator<FWD>&) noexcept; //!< iterator erase

public:
    size_t indexof(hash_t) const noexcept; //!< hash to index
    size_t size() const noexcept;          //!< total element count
    size_t capacity() const noexcept;      //!< get bucket count

public:
    bool reserve(size_t) noexcept; //!< reserve to power of 2
    void clear() noexcept;         //!< clear, but no shrink

public:
    Iterator<FWD> find(const T&) noexcept; //!< find by data
    Iterator<FWD> at(size_t) noexcept;     //!< find by order
    Iterator<FWD> begin() noexcept;        //!< get begin
    Iterator<FWD> end() noexcept;          //!< get end

public:
    Iterator<FWD | VIEW> find(const T&) const noexcept; //!< find by data const
    Iterator<FWD | VIEW> at(size_t) const noexcept;     //!< find by order const
    Iterator<FWD | VIEW> begin() const noexcept;        //!< get begin const
    Iterator<FWD | VIEW> end() const noexcept;          //!< get end const

private:
    template<typename U> bool emplace(U&&, hash_t);    //!< push detail
    void                      remove(Bucket*, Chain*); //!< pop detail

private:
    bool rehash(uint64_t); //!< bucket resize
    bool expand(Bucket*);  //!< chain resize

public:
    const Bucket* bucket(size_t) const noexcept; // get bucket(index)

private:
    Bucket*       slot(hash_t) noexcept;                 //!< get bucket slot
    Chain*        slot(hash_t, const T&) noexcept;       //!< get chain slot
    const Bucket* slot(hash_t) const noexcept;           //!< get bucket slot
    const Chain*  slot(hash_t, const T&) const noexcept; //!< get chain slot

private:
    size_t counter   = 0;                    //!< element counter
    size_t capacitor = 0;                    //!< bucket counter
    size_t factor    = 0;                    //!< load factor
    size_t log       = config::CAPACITY_LOG; //!< log2(capacitor): default (1 << 3) == 8

private:
    Bucket* buckets = nullptr;
    Grower  grower;

public:
    const float LOAD_FACTOR;
};

} // namespace container
LWE_END
#include "hashtable.ipp"
#endif
