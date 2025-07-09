LWE_BEGIN
namespace meta {

Decoder::Decoder(string_view in): str(in.data()), len(0), esc(0), depth(0) { }

template<typename T> bool Decoder::next() {
    if(str[len] == '\0') {
        return false;
    }

    str += len; // move (initial == str + 0)
    len  = 0;   // init
    esc  = 0;   // init

    // check
    if constexpr(std::is_same_v<string, T>) {
        if(str[len] != '\"') throw diag::error(diag::INVALID_DATA);
    }
    if constexpr(isSTL<T>()) {
        if(str[len] != '[') throw diag::error(diag::INVALID_DATA);
    }
    if constexpr(isOBJ<T>() || isKVP<T>()) {
        if(str[len] != '{') throw diag::error(diag::INVALID_DATA);
    }

    while(true) {
        ++len; // first -> pass
        const char& curr = str[len];
        if(curr == '\0') {
            break;
        }

        else if(curr == '\\') {
            ++esc; // check
        }
        else {
            // string check ""
            if constexpr(std::is_same_v<string, T>) {
                if(curr == '\"' && !(esc & 1)) {
                    ++len; // pass `\"`
                    break; // not escape sequence
                }
            }
            // container check []
            else if constexpr(isSTL<T>()) {
                if(curr == '[' && !(esc & 1)) {
                    ++depth; // check
                }
                else if(curr == ']' && !(esc & 1)) {
                    if(depth == 0) {
                        ++len; // pass `]`
                        break; // escape sequence
                    }
                    --depth;
                }
            }
            // object or depth check {}
            else if constexpr(isOBJ<T>() || isKVP<T>()) {
                if(curr == '{' && !(esc & 1)) {
                    ++depth; // check
                }
                else if(curr == '}' && !(esc & 1)) {
                    if(depth == 0) {
                        ++len; // pass `}`
                        break; // escape sequence
                    }
                    --depth; // check
                }
            }
            // pod types
            else if(curr == ',' && !(esc & 1)) break; // escape sequence

            // else
            esc = 0; // other character: init escape sequence counter
            continue;
        }
    }

    return true;
}

bool Decoder::next(const Type& in) {
    // check object
    if(in == Keyword::CLASS) {
        static const hash_t UNREGISTERED_CLASS = Type{ Keyword::CLASS }.hash();
        if(in.hash() == UNREGISTERED_CLASS) {
            throw diag::error(diag::INVALID_DATA); // unregistered type
        }
        return next<Object>();
    }
    // check container
    else if(isSTL(static_cast<Keyword>(in))) {
        return next<Container>();
    }
    // check string
    else if(in == Keyword::STD_STRING) {
        return next<string>();
    }
    // check pair
    else if(in == Keyword::STL_PAIR) {
        return next<KeyValue>();
    }
    return next<void>();
}

template<typename T> bool Decoder::check() {
    // string is no overlap, so no need to check
    // pair is fixed object count, so no need to check
    if constexpr(isOBJ<T>()) {
        if(str[len - 1] == '}') return false; // // find }, or }\0
    }
    else if constexpr(isSTL<T>()) {
        if(str[len - 1] == ']') return false; // find ], or ]\0
    }
    return str[len] != '\0'; // check end of string
}

bool Decoder::check(const Type& in) {
    // check object
    if(in == Keyword::CLASS) {
        static const hash_t UNREGISTERED_CLASS = Type{ Keyword::CLASS }.hash();
        if(in.hash() == UNREGISTERED_CLASS) {
            throw diag::error(diag::INVALID_DATA); // unregistered type
        }
        return check<Object>();
    }
    // check container
    else if(isSTL(static_cast<Keyword>(in))) {
        return check<Container>();
    }
    else return check<void>();
}

const string_view Decoder::get() {
    if(len == 0) {
        return string_view{ str, size_t(2) }; // [] or {} for container and object, else error
    }
    return string_view{ str, size_t(len) };
}

void Decoder::move(int begin) {
    str += begin;
}

void Decoder::trim(int end) {
    len += end;
}

} // namespace meta
LWE_END
