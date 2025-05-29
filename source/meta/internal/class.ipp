#ifdef LWE_META_CLASS

#include "object.hpp"
#include "feature.hpp"

LWE_BEGIN
namespace meta {

// default: object
const char* Class::name() const {
    return "Object";
}

// default: object
size_t Class::size() const {
    return sizeof(Object);
}

// default: object
const Structure& Class::fields() const {
    return Structure::reflect<Object>();
}

// default: object
const Class* Class::base() const {
    return nullptr;
}

// default: object
const Object* Class::statics() const {
    return meta::statics<Object>();
}

}
LWE_END
#endif