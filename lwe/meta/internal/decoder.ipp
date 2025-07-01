LWE_BEGIN
namespace meta {

Decoder::Decoder(const string& in): Decoder(in.c_str()) { }

Decoder::Decoder(const char* in): ptr(in), index(0), len(0), esc(0) { }

template<typename T> bool Decoder::next() {
    if(ptr[index] == '\0') {
        return false;
    }

    // check
    if constexpr(std::is_same_v<string, T>) {
        if(ptr[index] != '\"') throw diag::error(diag::INVALID_DATA);
    }
    if constexpr(isSTL<T>()) {
        if(ptr[index] != '[') throw diag::error(diag::INVALID_DATA);
    }
    if constexpr(isOBJ<T>() || isPAIR<T>()) {
        if(ptr[index] != '{') throw diag::error(diag::INVALID_DATA);
    }

    ptr   += index; // move (initial == ptr + 0)
    index  = 0;     // init
    esc    = 0;     // init

    while(true) {
        ++index; // first -> pass
        const char& curr = ptr[index];
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
        if(*type != "class") {
            return next<Object>();
        }
        throw diag::error(diag::INVALID_DATA); // unregistered type
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
    return ptr[index] != '\0';
}

string Decoder::get() {
    if(!ptr || index <= 0) {
        throw diag::error(diag::INVALID_DATA);
    }
    return string(ptr, len);
}

void Decoder::move(int in) {
    ptr += in;
}

void Decoder::trim(int in) {
    len += in;
}

} // namespace meta
LWE_END
