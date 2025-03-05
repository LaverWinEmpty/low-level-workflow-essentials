#ifndef LWE_CONTAINER_HEADER
#define LWE_CONTAINER_HEADER

#include "hal.hpp"

#define DECLARE_CONTAINER_TESTER(CONTAINER)                                                                            \
    template<typename, typename = std::void_t<>> struct Is##CONTAINER: std::false_type {};                             \
    template<typename T> struct Is##CONTAINER<T, std::void_t<typename T::CONTAINER##Element>>: std::true_type {};

#define DECLARE_CONTAINER(CONTAINER, ELEMENT)                                                                          \
    virtual const char* typestring() const override {                                                                  \
        return #CONTAINER;                                                                                             \
    }                                                                                                                  \
    virtual std::string tostring() const override {                                                                    \
        std::string out;                                                                                               \
        Iterator    curr = begin();                                                                                    \
        Iterator    last = end();                                                                                      \
        if(curr != last) {                                                                                             \
            out.append("{ ");                                                                                          \
            while(true) {                                                                                              \
                serialize(&out, &*curr, typecode<ELEMENT>());                                                          \
                ++curr;                                                                                                \
                if(curr != last) {                                                                                     \
                    out.append(", ");                                                                                  \
                } else break;                                                                                          \
            }                                                                                                          \
            out.append(" }");                                                                                          \
        } else return "{}";                                                                                            \
        return out;                                                                                                    \
    }                                                                                                                  \
    using CONTAINER##Element = ELEMENT

LWE_BEGIN

namespace stl {

namespace config {
static constexpr size_t DEF_SVO = 8; //!< default small vector optimization size
} // namespace config
using namespace config;

struct Container {
    Container() noexcept {}
    Container(const Container& in) noexcept {}
    Container(Container&& in) noexcept {}
    Container& operator=(const Container& in) noexcept { return *this; }
    Container& operator=(Container&& in) noexcept { return *this; }
    virtual ~Container() noexcept {}

public:
    virtual string      tostring() const = 0;
    virtual void        append(const std::string&) const {};
    virtual void        set(const std::string&) const {};
    virtual const char* typestring() const = 0;
};

} // namespace stl

LWE_END

#endif