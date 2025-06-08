#ifndef LWE_META_REGISTRY
#define LWE_META_REGISTRY

#include "../../base/base.h"

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
    using Table = std::unordered_map<string, T*>;

public:
    ~Registry();
    template<typename U> static U* add(const string&); //!< @tparam U base of T
    template<typename U> static U* add(const char*);   //!< @tparam U base of T
    static T*                      find(const char*);
    static T*                      find(const string&);

private:
    Table table;

private:
    static Table& instance();
};

}
LWE_END
#include "registry.ipp"
#endif