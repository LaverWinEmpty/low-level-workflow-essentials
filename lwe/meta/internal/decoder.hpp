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
    template<typename T> bool next();
    bool                      next(const Type&);

public:
    bool check();

public:
    const string_view get();

public:
    void move(int);
    void trim(int);

private:
    const char* str;

private:
    int index; //!< read size
    int esc;   //!< escape sequence find
    int len;   //!< size to read
};

} // namespace meta
LWE_END
#include "decoder.ipp"
#endif
