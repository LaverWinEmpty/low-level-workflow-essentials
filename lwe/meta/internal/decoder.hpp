#ifndef LWE_META_DECODER
#define LWE_META_DECODER

#include "../type.hpp"

LWE_BEGIN
namespace meta {

struct Decoder {
    Decoder(const string_view);

public:
    class Iterator;

public:
    // proxy
    struct String {
        const char* begin;
        size_t      size;
    };

public:
    template<typename T> bool next();            //!< T is data type
    bool                      next(const Type&); //!< Type is data type

public:
    template<typename T> bool check();            //!< T is outer type
    bool                      check(const Type&); //!< Type is outer type

public:
    const string_view get();

public:
    void move(int);
    void trim(int);

private:
    const char* str;

public:
    int len;   //!< size to read
    int esc;   //!< escape sequence find
    int depth; //!< brace count
};

} // namespace meta
LWE_END
#include "decoder.ipp"
#endif
