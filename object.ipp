#ifdef LWE_OBJECT_HEADER

std::string Object::serialize() const {

    const Structure& prop = meta()->fields();
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

    // empty
    if(in == "{}") {
        return;
    }

    const Structure& prop = meta()->fields();
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

template<typename T> bool Object::isA() const {
    Class* cls = classof<T>();
    if(cls) {
        Class* self = meta();
        while(cls) {
            if(cls == self) {
                return true;
            }
            cls = cls->base();
        }
    }
    return false;
}

void Object::deserialize(Object* out, const std::string& in) {
    out->deserialize(in);
}

const char* ObjectMeta::name() const {
    return "Object";
}

size_t ObjectMeta::size() const {
    return sizeof(Object);
}

const Structure& ObjectMeta::fields() const {
    return Structure::reflect<Object>();
}

Class* ObjectMeta::base() const {
    return nullptr;
}

Class* Object::meta() const {
    return classof<Object>();
}

Registered Object_REGISTERED = registclass<Object>();

#endif