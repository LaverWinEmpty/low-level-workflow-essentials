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

const const Field& Class::field(const char* name) const {
    return field(string{ name });
}

const const Field& Class::field(const string& name) const {
    static const Field failed = {
        .type   = Type{},
        .name   = nullptr,
        .size   = 0,
        .offset = size_t(-1)
    };

    const Structure& temp = fields();
    for(auto& itr : temp) {
        if(itr.name == name) {
            return itr;
        }
    }
    return failed;
}

}
LWE_END
#endif