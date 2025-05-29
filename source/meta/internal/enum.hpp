#ifndef LWE_META_ENUM
#define LWE_META_ENUM

#include "reflector.hpp"

LWE_BEGIN
namespace meta {

//! @brief enum info
struct Enumerator {
    uint64_t    value;
    const char* name;
};

/// @brief enum metadata
struct Enum {
    virtual const char*      name() const  = 0;
    virtual size_t           size() const  = 0;
    virtual const Enumerate& enums() const = 0;

public:
    template<typename E> static const char* serialize(E);
    static const char*                      serialize(const string&, uint64_t);
    static const char*                      serialize(const char*, uint64_t);

public:
    template<typename E> static E deserialize(const char*);
    template<typename E> static E deserialize(const string&);
    static uint64_t               deserialize(const string&, const string&);
};

}
LWE_END
#endif