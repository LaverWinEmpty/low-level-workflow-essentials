#ifdef LWE_REFLECT_HEADER

//! @brief
template<typename T> FieldInfo reflect(std::initializer_list<MetaField> list) {
    static FieldInfo result;
    // check
    size_t loop = list.size();
    if(loop != 0 || result.size() == 0) {
        MetaClass* meta = MetaClass::get<T>();
        MetaClass* base = meta->base();

        // reserve
        MetaClass* parent = meta->base();
        if(parent) {
            result.reserve(loop + parent->field().size()); // for append
        } else result.reserve(loop);

        // declare append lambda
        std::function<void(FieldInfo&, const MetaClass*)> append = [&append](FieldInfo& out, const MetaClass* base) {
            if(!base) {
                return; // end
            }
            append(out, base->base()); // parent first
            for(auto& itr : base->field()) {
                result.emplace_back(itr); // append
            }
        };

        // call
        append(result, base);

        // set
        size_t offset = 0;
        if(base != nullptr) {
            offset = base->size(); // append
        } else if(std::is_polymorphic_v<T>) {
            offset = sizeof(void*); // pass vptr
        }

        // iterator
        MetaField*       itr = const_cast<MetaField*>(list.begin());
        MetaField*       old = itr;
        const MetaField* end = list.end();

        // first
        if(itr != end) {
            itr->offset  = offset;       // set
            offset      += itr->size;    // next
            result.emplace_back(*itr++); // add
        }
        while(itr != end) {
            // align
            offset = itr->size <= alignof(T) ? lwe::Common::align(offset, itr->size) :
                                               lwe::Common::align(offset, alignof(T));

            itr->offset  = offset;       // set
            offset      += itr->size;    // next
            result.emplace_back(*itr++); // add
        }
    }
    return result;
}

std::string Object::serialize() const {
    const FieldInfo& prop = metaclass()->field();

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

    const FieldInfo& prop = metaclass()->field();

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
                ;
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

#endif