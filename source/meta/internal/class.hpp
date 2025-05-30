#ifndef LWE_META_CLASS
#define LWE_META_CLASS

#include "reflector.hpp"

LWE_BEGIN
namespace meta {

class Object;

//! @brief member variable info
struct Field {
    Type        type;
    const char* name;
    size_t      size;
    size_t      offset;

    explicit operator bool() const { return name != nullptr; }
};

/// @brief class metadata
struct Class {
    virtual const char*      name() const;
    virtual size_t           size() const;
    virtual const Structure& fields() const;
    virtual const Class*     base() const;
    virtual const Object*    statics() const;

public:
    const Field& field(const char*) const;
    const Field& field(const string&) const;
};

}
LWE_END
#include "class.ipp"
#endif