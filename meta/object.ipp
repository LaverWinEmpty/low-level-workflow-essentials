#ifdef LWE_OBJECT

LWE_BEGIN
namespace meta {

Object* create(const Class* in) {
    size_t size   = in->size();
    auto   result = Object::pool().find(size);
    if(result == Object::pool().end()) {
        Object::pool()[size] = new mem::Pool(size, 1); // not aligned
    }
    Object* obj = static_cast<Object*>(result->second->allocate());
    // TODO: create initializer
    std::memcmp(obj, in->statics(), size);
    return obj;
}

void destroy(Object* in) {
    size_t size = in->meta()->size();
    in->~Object();
    Object::pool()[size]->deallocate<void>(in);
}

template<typename T> T* create() {
    if constexpr(!std::is_base_of_v<Object, T>) {
        assert(false);
        return nullptr;
    }
    size_t size   = classof<T>()->size();
    auto   result = Object::pool().find(size);
    if(result == Object::pool().end()) {
        Object::pool()[size] = new mem::Pool(size, 1); // not aligned
        return Object::pool()[size]->allocate<T>();
    }
    return result->second->allocate<T>();
}

template<typename T> void destroy(T* in) {
    if constexpr(!std::is_base_of_v<Object, T>) {
        assert(false);
    }
    size_t size = in->meta()->size();
    in->~T();
    Object::pool()[size]->deallocate<void>(in);
}

std::string Object::stringfy() const {
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
        serialize(&buffer, ptr + prop[i].offset, prop[i].type);
        buffer.append(", ");
    }
    serialize(&buffer, ptr + prop[loop].offset, prop[loop].type);
    buffer.append(" }");
    return buffer;
}

void Object::parse(const std::string& in) {
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
            deserialize(out + prop[i].offset, in.substr(begin, len + 1), prop[i].type);
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
            deserialize(out + prop[i].offset, in.substr(begin, len + 1), prop[i].type);
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
            deserialize(out + prop[i].offset, in.substr(begin, len + 1), prop[i].type);
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
            deserialize(out + prop[i].offset, in.substr(begin, len), prop[i].type); // ignore ',' or ' '
            begin += 3;                                                             // pass <, > or < ]>
            len    = 0;
        }
    }
}

Object::~Object() {}

template<typename T> bool Object::isof() const {
    const Class* cls = classof<T>();
    if(cls) {
        const Class* self = meta();
        while(self) {
            if(cls == self) {
                return true;
            }
            self = self->base();
        }
    }
    return false;
}

bool Object::isof(const Class* in) const {
    const Class* self = meta();
    while(self) {
        if(in == self) {
            return true;
        }
        self = self->base();
    }
    return false;
}

bool Object::isof(const char* in) const {

    return isof(string{ in });
}

bool Object::isof(const string& in) const {
    const Class* cls = classof(in);
    if(cls) {
        const Class* self = meta();
        while(self) {
            if(cls == self) {
                return true;
            }
            self = self->base();
        }
    }
    return false;
}

void Object::parse(Object* out, const std::string& in) {
    out->parse(in);
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

Object* ObjectMeta::statics() const {
    // return statics<Object>();
    return nullptr;
}

Class* Object::meta() const {
    return classof<Object>();
}

Registered Object_REGISTERED = registclass<Object>();

}
LWE_END
#endif