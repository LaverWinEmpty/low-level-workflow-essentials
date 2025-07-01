LWE_BEGIN
namespace meta {

Decoder::Decoder(string_view in): str(in.data()), index(0), len(0), esc(0) { }

template<typename T> bool Decoder::next() {
    if(str[index] == '\0') {
        return false;
    }

    // check
    if constexpr(std::is_same_v<string, T>) {
        if(str[index] != '\"') throw diag::error(diag::INVALID_DATA);
    }
    if constexpr(isSTL<T>()) {
        if(str[index] != '[') throw diag::error(diag::INVALID_DATA);
    }
    if constexpr(isOBJ<T>() || isPAIR<T>()) {
        if(str[index] != '{') throw diag::error(diag::INVALID_DATA);
    }

    str   += index; // move (initial == str + 0)
    index  = 0;     // init
    esc    = 0;     // init

    while(true) {
        ++index; // first -> pass
        const char& curr = str[index];
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
                    ++index; // pass `\"`
                    break;   // not escape sequence
                }
                else continue;
            }
            // container check []
            else if constexpr(isSTL<T>()) {
                if(curr == ']' && !(esc & 1)) {
                    ++index; // pass `]`
                    break;   // escape sequence
                }
                else continue;
            }
            // object or pair check {}
            else if constexpr(isOBJ<T>() || isPAIR<T>()) {
                if(curr == '}' && !(esc & 1)) {
                    ++index; // pass `}`
                    break;   // escape sequence
                }
                else continue;
            }
            // else
            else if(curr == ',' && !(esc & 1)) break; // escape sequence
            else {
                esc = 0; // other character: init escape sequence counter
                continue;
            }
        }
    }

    len = index; // set length
    return true;
}

bool Decoder::next(const Type& type) {
    // check object
    if(type == Keyword::CLASS) {
        static const hash_t UNREGISTERED_CLASS = Type{ Keyword::CLASS }.hash();

        if(type.hash() == UNREGISTERED_CLASS) {
            throw diag::error(diag::INVALID_DATA); // unregistered type
        }
        return next<Object>();
    }
    // check
    else if(type == Keyword::STD_STRING) {
        return next<string>();
    }
    // check container
    else if(isSTL(type)) {
        return next<Container>();
    }
    // check pair
    else if(type == Keyword::STL_PAIR) {
        return next<Pair>();
    }
    return next<void>();
}

bool Decoder::check() {
    return str[index] != '\0';
}

const string_view Decoder::get() {
    if(!str || index <= 0) {
        throw diag::error(diag::INVALID_DATA);
    }
    return string_view{ str, size_t(len) };
}

void Decoder::move(int in) {
    str += in;
}

void Decoder::trim(int in) {
    len += in;
}

} // namespace meta
LWE_END
