#ifndef LWE_META_DECODER
#define LWE_META_DECODER

#include "../type.hpp"

LWE_BEGIN
namespace meta {
struct Decoder {
    Decoder(const std::string&);
    Decoder(const char*);

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
    string get();

public:
    void move(int);
    void trim(int);

private:
    const char* ptr;

private:
    int index; //!< read size
    int len;   //!< size to read
    int esc;   //!< escape sequence find
};

} // namespace meta
LWE_END
#include "decoder.ipp"
#endif
