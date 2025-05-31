#ifndef LWE_META_REGISTRY
#define LWE_META_REGISTRY

#include "../../core/core.h"

LWE_BEGIN
namespace meta {

/// @brief static object registry
/// @note  Registry<Class> == metadata registry
/// @tparam T: Enum (enum metadata)
///            Class (class metadata) 
///            Object (static instance)
///            Method (method lambda)
template<typename T> class Registry {
    Registry() = default;

public:
    using Table = std::unordered_map<string, T*>;

public:
    ~Registry();
    template<typename U> static void add(const string&); //!< @tparam U base of T
    template<typename U> static void add(const char*);   //!< @tparam U base of T
    static T*                        find(const char*);
    static T*                        find(const string&);

private:
    Table table;

private:
    static Table& instance();
};

class Method;

/// @brief specialize
template<> class Registry<Method> {
    // template<typename T> friend Registered registmethod();
    Registry() = default;

public:
    ~Registry();

public:
    using Table = std::unordered_map<string, std::unordered_map<string, Method*>>;

public:
    static void add(const char*   cls, const char*   name, Method* in);
    static void add(const char*   cls, const string& name, Method* in);
    static void add(const string& cls, const char*   name, Method* in);
    static void add(const string& cls, const string& name, Method* in);

public:
    static Method* find(const char*   cls, const char*   name);
    static Method* find(const char*   cls, const string& name);
    static Method* find(const string& cls, const char*   name);
    static Method* find(const string& cls, const string& name);

private:
    Table table;

private:
    static Table& instance();
};

}
LWE_END
#include "registry.ipp"
#endif