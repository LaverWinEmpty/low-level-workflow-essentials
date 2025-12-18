#ifndef LWE_META_DECODER
#define LWE_META_DECODER

#include "../type.hpp"

LWE_BEGIN
namespace meta {

struct Decoder {
    Decoder(const StringView);

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
    const StringView get();

public:
    void move(int);
    void trim(int);

private:
    template<typename T> static constexpr bool isstr() { return std::is_same_v<String, T>; }
    template<typename T> static constexpr bool iscont() {
        if constexpr(std::is_same_v<T, Container>) {
            return true;
        }
        constexpr Keyword keyword = TypeEraser<T>::KEYWORD;
        return storable(keyword);
    }
    template<typename T> static constexpr bool isobj() {
        return std::is_base_of_v<Object, T> || std::is_same_v<Object, T>;
    }
    template<typename T> static constexpr bool ispair() { return TypeEraser<T>::KEYWORD == Keyword::STD_PAIR; }

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
