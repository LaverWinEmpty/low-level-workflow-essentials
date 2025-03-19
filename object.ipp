#ifdef LWE_OBJECT_HEADER

std::string Object::serialize() const {
    const Structure& prop = metaclass()->fields();
    if(prop.size() == 0) {
        return {};
    }

    std::string buffer;
    buffer.reserve(4096);

    char*  ptr  = const_cast<char*>(reinterpret_cast<const char*>(this));
    size_t loop = prop.size() - 1;
    buffer.append("{ ");
    for(size_t i = 0; i < loop; ++i) {
        ::serialize(&buffer, ptr + prop[i].offset, prop[i].type);
        buffer.append(", ");
    }
    ::serialize(&buffer, ptr + prop[loop].offset, prop[loop].type);
    buffer.append(" }");
    return buffer;
}

void Object::deserialize(const std::string& in) {
    char* out = const_cast<char*>(reinterpret_cast<const char*>(this));

    const Structure& prop = metaclass()->fields();
    if(prop.size() == 0) {
        assert(false);
    }

    size_t begin = 2; // "{ ", ignore 2
    size_t len   = 0;

    size_t loop = prop.size();
    for(size_t i = 0; i < loop; ++i) {
        if(isSTL(prop[i].type.type())) {
            len = 1; // pass '['
            while(true) {
                if(in[begin + len] == ']' && in[begin + len - 1] != '\\') {
                    break;
                }
                ++len;
            }
            // len + 1: with ']'
            ::deserialize(out + prop[i].offset, in.substr(begin, len + 1), prop[i].type);
            begin += (len + 3); // pass <], >
            len    = 0;
        }

        else if(prop[i].type == EType::STD_STRING) {
            len = 1; // pass '\"'
            while(true) {
                if(in[begin + len] == '\"' && in[begin + len - 1] != '\\') {
                    break;
                }
                ++len;
            }
            // len + 1: with '\"'
            ::deserialize(out + prop[i].offset, in.substr(begin, len + 1), prop[i].type);
            begin += (len + 3); // pass <", >
            len    = 0;
        }

        else if(prop[i].type == EType::CLASS) {
            len = 1; // pass "{ "

            while(true) {
                if(in[begin + len] == '}' && in[begin + len - 1] != '\\') {
                    break;
                }
                ++len;
            }
            // len + 1: with '}'
            ::deserialize(out + prop[i].offset, in.substr(begin, len + 1), prop[i].type);
            begin += (len + 3); // pass <}, >
            len    = 0;
        }

        // primitive: integer or floating
        else {
            // find <, > or < }>
            while(true) {
                if((in[begin + len] == ',' && in[begin + len + 1] == ' ') ||
                   (in[begin + len] == ' ' && in[begin + len + 1] == '}')) {
                    break;
                }
                ++len;
            }
            ::deserialize(out + prop[i].offset, in.substr(begin, len), prop[i].type); // ignore ',' or ' '
            begin += 3;                                                               // pass <, > or < ]>
            len    = 0;
        }
    }
}

void Object::deserialize(Object* out, const std::string& in) {
    out->deserialize(in);
}

template<> MetaClass* MetaClass::get<Object>() {
    Object object;
    return object.metaclass();
}

template<> const Structure& Structure::reflect<Object>() {
    static Structure EMPTY; // default
    return EMPTY;
}

const char* Object::ObjectMeta::name() const {
    return "Object";
}

size_t Object::ObjectMeta::size() const {
    return sizeof(Object);
}

const Structure& Object::ObjectMeta::fields() const {
    return Structure::reflect<Object>();
}

MetaClass* Object::ObjectMeta::base() const {
    return nullptr;
}

#endif