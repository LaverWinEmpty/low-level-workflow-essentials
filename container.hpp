#ifndef LWE_CONTAINER_HEADER
#define LWE_CONTAINER_HEADER

#include "hal.hpp"

#define CONTAINER_ARRAY_BODY(CONTAINER, ELEMENT, SVO)                                                                  \
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
    virtual string serialize() const                        = 0;
    virtual void   deserialize(const string&, bool = false) = 0;

protected:
    //! @brief deserialized data load
    virtual void load(const void*) = 0;

protected:
    template<typename Container> string serialization() const {
        using Element = typename Container::value_type;

        std::string out;

        typename Container::Iterator curr = static_cast<const Container*>(this)->begin();
        typename Container::Iterator last = static_cast<const Container*>(this)->end();
        if(curr != last) {

            out.append("{ ");
            while(true) {

                tostr(&out, &*curr, typecode<Element>());
                ++curr;
                if(curr != last) {

                    out.append(", ");
                } else break;
            }
            out.append(" }");
        } else return "{}";
        return out;
    }

    template<typename Container> void deserialization(const string& in, bool append) {
        using Element = typename Container::value_type;

        if(in == "{}") {
            return; // empty
        }

        size_t stack = 1;
        size_t begin = 2;             // "{ ", ignore 2
        size_t end   = in.size() - 2; // " }", ignore 2
        size_t len   = 0;
        size_t pair  = 0;

        if(!append) {
            static_cast<Container*>(this)->clear();
        }

        // parsing
        size_t i = begin;
        for(; i < end; ++i, ++len) {
            if constexpr(std::is_same_v<Element, string>) {
                // find [",]
                if(in[i] == '\"' && in[i + 1] == ',') {
                    Element data;

                    // len + 1: ignore '\"'
                    fromstr(reinterpret_cast<void*>(&data), in.substr(begin, len + 1), typecode<Element>());
                    i     += 3; // pass [", ]
                    begin  = i; // next position
                    len    = 0; // next length
                    load(reinterpret_cast<const void*>(&data));
                }
            }

            else if constexpr(isSTL<Element>()) {
                // find [},]
                if(in[i] == '}' && in[i + 1] == ',') {
                    Element data;

                    // len + 1: ignore '{'
                    fromstr(reinterpret_cast<void*>(&data), in.substr(begin, len + 1), typecode<Element>());
                    i     += 3; // pass [}, ]
                    begin  = i; // next position
                    len    = 0; // next length
                    load(reinterpret_cast<const void*>(&data));
                }
            }

            else if(in[i] == ',') {
                Element data;
                fromstr(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
                i     += 2; // pass [, ]
                begin  = i; // next position
                len    = 0; // next length
                load(reinterpret_cast<const void*>(&data));
            }
        }

        Element data;
        fromstr(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
        load(reinterpret_cast<const void*>(&data));
    }
};

} // namespace stl

LWE_END

#endif