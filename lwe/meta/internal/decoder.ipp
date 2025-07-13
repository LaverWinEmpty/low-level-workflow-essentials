LWE_BEGIN
namespace meta {

Decoder::Decoder(StringView in): str(in.data()), len(0), esc(0), depth(0) { }

template<typename T> bool Decoder::next() {
    if(str[len] == '\0') {
        return false;
    }

    str += len; // move (initial == str + 0)
    len  = 0;   // init
    esc  = 0;   // init

    // check
    if constexpr(isstr<T>()) {
        if(str[len] != '\"') throw diag::error(diag::INVALID_DATA);
    }
    if constexpr(iscont<T>()) {
        if(str[len] != '[') throw diag::error(diag::INVALID_DATA);
    }
    if constexpr(isobj<T>() || ispair<T>()) {
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
            if constexpr(isstr<T>()) {
                if(curr == '\"' && !(esc & 1)) {
                    ++len; // pass `\"`
                    break; // not escape sequence
                }
            }
            // container check []
            else if constexpr(iscont<T>()) {
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
            if constexpr(isobj<T>() || ispair<T>()) {
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
    else if(storable(static_cast<Keyword>(in))) {
        return next<Container>();
    }
    // check string
    else if(in == Keyword::STD_STRING) {
        return next<String>();
    }
    // check pair
    else if(in == Keyword::STD_PAIR) {
        return next<std::pair<int, int>>(); // use {}
    }
    return next<void>();
}

template<typename T> bool Decoder::check() {
    // string is no overlap, so no need to check
    // pair is fixed object count, so no need to check
    if constexpr(isobj<T>() || ispair<T>()) {
        if(str[len - 1] == '}') return false; // // find }, or }\0
    }
    else if constexpr(iscont<T>()) {
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
    else if(storable(static_cast<Keyword>(in))) {
        return check<Container>();
    }
    else return check<void>();
}

const StringView Decoder::get() {
    if(len == 0) {
        return StringView{ str, size_t(2) }; // [] or {} for container and object, else error
    }
    return StringView{ str, size_t(len) };
}

void Decoder::move(int begin) {
    str += begin;
}

void Decoder::trim(int end) {
    len += end;
}

} // namespace meta
LWE_END
