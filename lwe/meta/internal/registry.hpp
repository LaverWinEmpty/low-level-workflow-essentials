#ifndef LWE_META_REGISTRY
#define LWE_META_REGISTRY

#include "../../base/base.h"
#include "../../container/hash_table.hpp"

LWE_BEGIN
namespace meta {

//! @brief static object registry
//! @note  Registry<Class> == metadata registry
//! @tparam T: Enum (enum metadata)
//!            Class (class metadata)
//!            Object (static instance)
//!            Method (method lambda)
template<typename T> class Registry {
    Registry() = default;

public:
    using Table = container::HashTable<String, T*>;

public:
    ~Registry();
    template<typename U> static void add(const String&);  //!< @tparam U base of T
    template<typename U> static void add(const char*);    //!< @tparam U base of T
    static T*                        find(const String&); //!< find, compare string, no string_view
    static T*                        find(const char*);   //!< find, compare string, no string_view

private:
    Table table;

private:
    static Table& instance();
};

} // namespace meta
LWE_END
#include "registry.ipp"
#endif
