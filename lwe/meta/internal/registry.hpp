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
    //! @tparam U base of T, Args: U(args...)
    //! @note   e.g. T == Object, U == Obejct derived class
    template<typename U, typename... Args> static void add(const string&, Args&&...);

public:
    //! @tparam U base of T, Args: U(args...)
    //! @note   e.g. T == Object, U == Obejct derived class
    template<typename U, typename... Args> static void add(const char*, Args&&...);

public:
    static T* find(const char*);
    static T* find(const string&);

private:
    Table table;

private:
    static Table& instance();
};

}
LWE_END
#include "registry.ipp"
#endif