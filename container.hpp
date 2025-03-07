#ifndef LWE_CONTAINER_HEADER
#define LWE_CONTAINER_HEADER

#include "hal.hpp"

#define DECLARE_CONTAINER(CONTAINER, ELEMENT)                                                                          \
    virtual std::string serialize() const override {                                                                   \
        std::string out;                                                                                               \
        Iterator    curr = begin();                                                                                    \
        Iterator    last = end();                                                                                      \
        if(curr != last) {                                                                                             \
            out.append("{ ");                                                                                          \
            while(true) {                                                                                              \
                tostr(&out, &*curr, typecode<ELEMENT>());                                                              \
                ++curr;                                                                                                \
                if(curr != last) {                                                                                     \
                    out.append(", ");                                                                                  \
                } else break;                                                                                          \
            }                                                                                                          \
            out.append(" }");                                                                                          \
        } else return "{}";                                                                                            \
        return out;                                                                                                    \
    }                                                                                                                  \
    using CONTAINER##Element = ELEMENT;                                                                                \
    using value_type         = ELEMENT

LWE_BEGIN

namespace stl {

namespace config {
static constexpr size_t DEF_SVO = 8; //!< default small vector optimization size
} // namespace config
using namespace config;

struct Container {
    virtual ~Container() noexcept {}
    virtual string      serialize() const                        = 0;
    virtual void        deserialize(const string&, bool = false) = 0;
};

} // namespace stl

LWE_END

#endif