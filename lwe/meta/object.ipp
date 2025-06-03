#ifdef LWE_OBJECT
LWE_BEGIN
namespace meta {

async::Lock Object::lock;

// feauter.ipp implementation
template<typename T> Registered registclass() {
   // default, other class -> template specialization
   Structure::reflect<Object>();
   Registry<Object>::add<Object>("Object");
   Registry<Class>::add<ObjectMeta>("Object");
   return Registered::REGISTERED;
}

/*
 * object methods
 */

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
        meta::serialize(&buffer, ptr + prop[i].offset, prop[i].type.code());
        buffer.append(", ");
    }
    meta::serialize(&buffer, ptr + prop[loop].offset, prop[loop].type.code());
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
        if(isSTL(prop[i].type.code())) {
            len = 1; // pass '['
            while(true) {
                if(in[begin + len] == ']' && in[begin + len - 1] != '\\') {
                    break;
                }
                ++len;
            }
            // len + 1: with ']'
            meta::deserialize(out + prop[i].offset, in.substr(begin, len + 1), prop[i].type.code());
            begin += (len + 3); // pass <], >
            len    = 0;
        }

        else if(prop[i].type == Keyword::STD_STRING) {
            len = 1; // pass '\"'
            while(true) {
                if(in[begin + len] == '\"' && in[begin + len - 1] != '\\') {
                    break;
                }
                ++len;
            }
            // len + 1: with '\"'
            meta::deserialize(out + prop[i].offset, in.substr(begin, len + 1), prop[i].type.code());
            begin += (len + 3); // pass <", >
            len    = 0;
        }

        else if(prop[i].type == Keyword::CLASS) {
            len = 1; // pass "{ "

            while(true) {
                if(in[begin + len] == '}' && in[begin + len - 1] != '\\') {
                    break;
                }
                ++len;
            }
            // len + 1: with '}'
            meta::deserialize(out + prop[i].offset, in.substr(begin, len + 1), prop[i].type.code());
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
            meta::deserialize(out + prop[i].offset, in.substr(begin, len), prop[i].type.code()); // ignore ',' or ' '
            begin += 3;                                                                          // pass <, > or < ]>
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

void Object::deserialize(Object* out, const std::string& in) {
    out->deserialize(in);
}

Class* Object::meta() const {
    return classof<Object>();
}

/*
 * object meta source
 */

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
    return meta::statics<Object>();
}

Registered Object_REGISTERED = registclass<Object>();

/*
 * etc
 */

Object* create(const Class* in) {
    size_t size   = in->size();
    auto   result = Object::pool().find(size);
    if(result == Object::pool().end()) {
        Object::pool()[size] = new mem::Pool(size, 1); // not aligned
    }
    Object* obj = static_cast<Object*>(result->second->allocate());
    // TODO: create initializer
    int discard = std::memcmp(obj, in->statics(), size);
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

    T* ptr = nullptr;
    LOCKGUARD(Object::lock) {
        size_t size   = classof<T>()->size();
        auto   result = Object::pool().find(size);
        if(result == Object::pool().end()) {
            Object::pool()[size] = new mem::Pool(size);
            return Object::pool()[size]->allocate<T>();
        }
        ptr = result->second->allocate<T>();
    }
    return ptr;
}

template<typename T> void destroy(T* in) {
    if constexpr(!std::is_base_of_v<Object, T>) {
        assert(false);
    }

    LOCKGUARD(Object::lock) {
        size_t size = in->meta()->size();
        in->~T();
        Object::pool()[size]->deallocate<void>(in);
    }
}

/*
 * serialize functions
 */

// primitive type to string
template<typename T> string serialize(const T& in) {
    std::stringstream ss;
    if constexpr(std::is_same_v<T, float>)       ss << std::fixed << std::setprecision(6) << in;
    else if constexpr(std::is_same_v<T, double>) ss << std::fixed << std::setprecision(14) << in;
    else                                         ss << in;
    return ss.str();
}

// boolean type to string
template<> string serialize<bool>(const bool& in) {
    return in ? "true" : "false";
}

// string to string
template<> string serialize<string>(const string& in) {
    string out;
    out.append("\"");
    size_t loop = in.size();
    for(size_t i = 0; i < loop; ++i) {
        switch(in[i]) {
            case '\\': out.append("\\\\");   break;
            case '\"': out.append("\\\"");   break;
            case '\n': out.append("\\n");    break;
            case '\t': out.append("\\t");    break;
            case '[': out.append("\\[");    break;
            case ']': out.append("\\]");    break;
            case '{': out.append("\\{");    break;
            case '}': out.append("\\}");    break;
            default:   out.append(1, in[i]); break;
          }
    }
    out.append("\"");
    return out;
}

// container to string
template<> string serialize<Container>(const Container& in) {
    return in.serialize(); // virtual
}

// container to string
template<typename T> string serialize(const Container* in) {
    std::string out;

    // CRTP begin / end
    typename T::Iterator curr = reinterpret_cast<const T*>(in)->begin();
    typename T::Iterator last = reinterpret_cast<const T*>(in)->end();

    // has data
    if(curr != last) {
        out.append("[");
        // for each
        while(true) {
            serialize(&out, &*curr, typecode<typename T::value_type>());
            ++curr;
            if(curr != last) {
                out.append(", ");
            } else break;
        }
        out.append("]");
    } else return "[]";
    return out;
} 

// string to primitive type
template<typename T> T deserialize(const string& in) {
    // TODO: exeption
    // 1. out of range
    // 2. format mismatch
    // 3. type mismatch ?

    size_t pos = 0;
    while (true) {
        if (in[pos] == '\0' || in[pos] == ',') {
            break;
        }
        ++pos;
    }
    std::stringstream ss{ in.substr(0, pos) };
    T result;
    ss >> result;
    return result;
}

// string to boolean type
template<> bool deserialize<bool>(const string& in) {
    // TODO: exeption not "true" or "false"

    if(in[0] == 't')  {
        return true;
    }
    return false;
}

// string to string
template<> string deserialize<string>(const string& in) {
    string result;
    if(in[0] != '\"') {
        assert(false);
    }
    size_t pos = in.rfind('\"');
    for(size_t i =  1; i < pos; ++i) {
        if(in[i] == '\\') {
            switch (in[i + 1]) {
                case '\\': result.append("\\"); break;
                case '\"': result.append("\""); break;
                case '[':  result.append("["); break;
                case ']':  result.append("]"); break;
                case '{':  result.append("{"); break;
                case '}':  result.append("}"); break;

                // \n, \t, etc...
                default:
                    result.append(1, '\\');
                    result.append(1, in[i + 1]);
                    break;
            }
            ++i;
        }
        else result.append(1, in[i]);
    }
    return result;
}

// string to container
void deserialize(Container* out, const string& in) {
    out->deserialize(in); // virtual
}

// string to container
template<typename Derived> void deserialize(Container* ptr, const string& in) {
    using Element = typename Derived::value_type;
    if(in == "[]") {
        return ; // empty
    }
    Derived& out = *reinterpret_cast<Derived*>(ptr); // else

    size_t begin = 1;             // "[", ignore 1
    size_t end   = in.size() - 1; // "]", ignore 1
    size_t len   = 0;

    // parsing
    size_t i = begin;
    for(; i < end; ++i, ++len) {
        if constexpr(std::is_same_v<Element, string>) {
            // find <",> but ignore \"
            if(in[i] == '\"' && in[i + 1] == ',', in[i - 1] != '\\') {
                Element data;
                // len + 1: with '\"'
                meta::deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len + 1), typecode<Element>());
                i     += 3; // pass <", >
                begin  = i; // next position
                len    = 0; // next length
                out.push(std::move(data));
            }
        }

        else if constexpr(isSTL<Element>()) {
            // find <],> but ignore \]
            if(in[i] == ']' && in[i + 1] == ',' && in[i - 1] != '\\') {
                Element data;
                // len + 1: with ']'
                meta::deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len + 1), typecode<Element>());
                i     += 3; // pass <], >
                begin  = i; // next position
                len    = 0; // next length
                out.push(std::move(data));
            }
        }

        else if constexpr(std::is_same_v<Element, Object> || std::is_base_of_v<Object, Element>) {
            if(in[i] == '}' && in[i + 1] == ',' && in[i - 1] != '\\') {
                Element data;
                meta::deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
                i     += 2; // pass <, >
                begin  = i; // next position
                len    = 0; // next length
                out.push(std::move(data));
            }
        }

        else if(in[i] == ',') {
            Element data;
            meta::deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
            i     += 2; // pass <, >
            begin  = i; // next position
            len    = 0; // next length
            out.push(std::move(data));
        }
    }

    // insert last data
    Element data;
    meta::deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
    out.push(std::move(data));
}

// runtime stringify
void serialize(std::string* out, const void* in, const Keyword& type) {
    switch (type) {
    // skip
    case Keyword::UNREGISTERED:
    case Keyword::VOID: 
        //TODO:
        break;

    case Keyword::CLASS:  
        out->append(reinterpret_cast<const Object*>(in)->serialize());
        break;

    case Keyword::POINTER: {
        //TODO:
        }
        break;

    case Keyword::REFERENCE: {
        //TODO:
        }
        break;

        out->append(serialize(*static_cast<const bool*>(in)));
        break;

    // int
    case Keyword::SIGNED_INT:
    case Keyword::UNSIGNED_INT:
        out->append(serialize(*static_cast<const int*>(in)));
        break;

    case Keyword::BOOL:
        out->append(serialize(*static_cast<const int*>(in)));
        break;

    // char
    case Keyword::CHAR:
    case Keyword::SIGNED_CHAR:
    case Keyword::UNSIGNED_CHAR:
        out->append(serialize(*static_cast<const char*>(in)));
        break;

    // wchar
    case Keyword::WCHAR_T:
        //TODO: fix
        // stringify<wchar_t>(out, in);
        break;

    // short
    case Keyword::SIGNED_SHORT:
    case Keyword::UNSIGNED_SHORT: 
        out->append(serialize(*static_cast<const short*>(in)));
        break;

    // long
    case Keyword::SIGNED_LONG:
    case Keyword::UNSIGNED_LONG: 
        out->append(serialize(*static_cast<const long*>(in)));
        break;

    // long long
    case Keyword::SIGNED_LONG_LONG:
    case Keyword::UNSIGNED_LONG_LONG:
        out->append(serialize(*static_cast<const long long*>(in)));
        break;
    
    // float
    case Keyword::FLOAT:
        out->append(serialize<float>(*static_cast<const float*>(in))); 
        break;

    // double
    case Keyword::DOUBLE: 
        out->append(serialize<double>(*static_cast<const double*>(in)));
        break;

    // long double
    case Keyword::LONG_DOUBLE: 
        out->append(serialize<long double>(*static_cast<const long double*>(in)));
        break;
    
    case Keyword::ENUM:
        // out->append(static_cast<const EInterface*>(in)->stringify());
        break;

    // function
    case Keyword::FUNCTION: 
        //TODO:
        break;

    case Keyword::STD_STRING: 
        out->append(serialize(*static_cast<const string*>(in)));
        break;

    case Keyword::STL_DEQUE:
        out->append(serialize(*static_cast<const Container*>(in)));
        break;
    }
}

// runtime parse
void deserialize(void* out, const std::string& in, const Keyword& type) {
    switch (type) {
    case Keyword::UNREGISTERED:
    case Keyword::VOID:
        break;

    case Keyword::CLASS: 
        Object::deserialize(static_cast<Object*>(out), in);
        break;

    case Keyword::POINTER: {
        //TODO:
        }
        break;

    case Keyword::REFERENCE: {
        //TODO:
        }
        break;

    case Keyword::UNION: {
        //TODO:
        }
        break;

    // int
    case Keyword::SIGNED_INT:
    case Keyword::UNSIGNED_INT: 
        *static_cast<int*>(out) = deserialize<int>(in);
        break;

    // bool
    case Keyword::BOOL:
        *static_cast<bool*>(out) = deserialize<bool>(in);
    break;

    // char
    case Keyword::CHAR:
    case Keyword::SIGNED_CHAR:
    case Keyword::UNSIGNED_CHAR:
        *static_cast<char*>(out) = deserialize<char>(in);
        break;

    // wchar
    case Keyword::WCHAR_T:
    // TODO: fix
        // parse<wchar_t>(static_cast<wchar_t*>(out), in, inout);
        break;

    // short
    case Keyword::SIGNED_SHORT:
    case Keyword::UNSIGNED_SHORT: 
        *static_cast<short*>(out) = deserialize<short>(in);
        break;

    // long
    case Keyword::SIGNED_LONG:
    case Keyword::UNSIGNED_LONG: 
        *static_cast<long*>(out) = deserialize<long>(in);
        break;

    // long long
    case Keyword::SIGNED_LONG_LONG:
    case Keyword::UNSIGNED_LONG_LONG:
        *static_cast<long long*>(out) = deserialize<long long>(in);
        break;

    // float
    case Keyword::FLOAT:
        *static_cast<float*>(out) = deserialize<float>(in);
        break;

    // double
    case Keyword::DOUBLE:
        *static_cast<double*>(out) = deserialize<double>(in);
        break;

    // long double    
    case Keyword::LONG_DOUBLE:
        *static_cast<long double*>(out) = deserialize<long double>(in);
        break;

    case Keyword::ENUM:
        // static_cast<EInterface*>(out)->parse(in);
        break;

    // function
    case Keyword::FUNCTION:
        //TODO:
        break;

    // std::string
    case Keyword::STD_STRING: 
        *static_cast<string*>(out) = deserialize<string>(in);
        break;

    case Keyword::STL_DEQUE:
        static_cast<Container*>(out)->deserialize(in);
        break;
    }
}

}
LWE_END
#endif