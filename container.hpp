#ifndef LWE_CONTAINER_HEADER
#define LWE_CONTAINER_HEADER

#include "hal.hpp"

#define CONTAINER_BODY(CONTAINER, ...)                                                                                 \
    using CONTAINER##Element = typename Container::Type<__VA_ARGS__>::Element;                                         \
    virtual void deserialize(const string& in) override {                                                              \
        *this = Container::deserialize<CONTAINER<__VA_ARGS__>>(in);                                                    \
    }                                                                                                                  \
    virtual std::string serialize() const override {                                                                   \
        return Container::serialize<CONTAINER<__VA_ARGS__>>(this);                                                     \
    }                                                                                                                  \
    virtual void parsing(void* in) override {                                                                          \
        push(std::move(*static_cast<CONTAINER##Element*>(in)));                                                        \
    }                                                                                                                  \
    using value_type = typename CONTAINER##Element

LWE_BEGIN

namespace stl {

namespace config {
static constexpr size_t DEF_SVO = 8; //!< default small vector optimization size
} // namespace config
using namespace config;

struct Container {
    virtual ~Container() noexcept {}
    virtual string serialize() const          = 0;
    virtual void   deserialize(const string&) = 0;
    virtual void   parsing(void*)             = 0; //!< use on deserialize, e.g. { push(*(T*)in); }

protected:
    template<typename Container> static string serialize(const Container* in) {
        using Element = typename Container::value_type;
        std::string                  out;
        typename Container::Iterator curr = in->begin();
        typename Container::Iterator last = in->end();
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

public:
    template<typename Container> static Container deserialize(const string& in) {
        using Element = typename Container::value_type;
        Container out;
        if(in == "{}") {
            return out; // empty
        }

        size_t stack = 1;
        size_t begin = 2;             // "{ ", ignore 2
        size_t end   = in.size() - 2; // " }", ignore 2
        size_t len   = 0;
        size_t pair  = 0;

        // parsing
        size_t i = begin;
        for(; i < end; ++i, ++len) {
            if constexpr(std::is_same_v<Element, string>) {
                // find [",]
                if(in[i] == '\"' && in[i + 1] == ',') {
                    Element data;
                    // len + 1: with '\"'
                    fromstr(reinterpret_cast<void*>(&data), in.substr(begin, len + 1), typecode<Element>());
                    i     += 3; // pass [", ]
                    begin  = i; // next position
                    len    = 0; // next length
                    out.parsing(reinterpret_cast<void*>(&data));
                }
            }

            else if constexpr(isSTL<Element>()) {
                // find [},]
                if(in[i] == '}' && in[i + 1] == ',') {
                    Element data;
                    // len + 1: with '}'
                    fromstr(reinterpret_cast<void*>(&data), in.substr(begin, len + 1), typecode<Element>());
                    i     += 3; // pass [}, ]
                    begin  = i; // next position
                    len    = 0; // next length
                    out.parsing(reinterpret_cast<void*>(&data));
                }
            }

            else if(in[i] == ',') {
                Element data;
                fromstr(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
                i     += 2; // pass [, ]
                begin  = i; // next position
                len    = 0; // next length
                out.parsing(reinterpret_cast<void*>(&data));
            }
        }

        Element data;
        fromstr(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
        out.parsing(reinterpret_cast<void*>(&data));

        return std::move(out);
    }

public:
    template<typename T, size_t...> struct Type {
        using Element = T;
    };
};

} // namespace stl

LWE_END

#endif