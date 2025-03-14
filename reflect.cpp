#include "meta.hpp"

class TestA {
    double d;
    char   test[83];
    int    v;
};

//! @brief
template<typename T> std::vector<MetaField> reflect(std::initializer_list<MetaField> list) {
    static std::vector<MetaField> result;
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
        std::function<void(std::vector<MetaField>&, const MetaClass*)> append = [&append](std::vector<MetaField>& out,
                                                                                          const MetaClass* base) {
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

//! @brief
class Object {
    friend struct MetaClass;
    struct ObjectMeta: MetaClass {
        virtual const char*      name() const override { return "Object"; }
        virtual size_t           size() const override { return sizeof(Object); }
        virtual const FieldInfo& field() const override {
            static const FieldInfo EMPTY; // default
            return EMPTY;
        }
        virtual MetaClass* base() const override { return nullptr; }
    };

public:
    virtual MetaClass* metaclass() {
        static ObjectMeta meta;
        return &meta;
    }
    virtual MetaClass* metabase() { return nullptr; }

public:
    std::string serialize() {
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

public:
    void deserialize(const std::string& in) {
        char* out = const_cast<char*>(reinterpret_cast<const char*>(this));

        const FieldInfo& prop = metaclass()->field();
        
        size_t begin  = 2;     // "{ ", ignore 1
        size_t len    = 0;

        size_t loop = prop.size();
        for (size_t i = 0; i < loop; ++i) {
            if (isSTL(prop[i].type.type())) {
                while (in[begin + len] != ']' && in[begin + len - 1] != '\\') {
                    ++len;
                }
                // len + 1: with ']'
                ::deserialize(out + prop[i].offset, in.substr(begin, len + 1), prop[i].type);
                begin += (len + 3); // pass <], >
                len = 0;
            }

            else if (prop[i].type == EType::STD_STRING) {
                len = 1; // pass '\"'
                while (in[begin + len] != '\"' && in[begin + len - 1] != '\\') {
                    ++len;
                }
                // len + 1: with '\"'
                ::deserialize(out + prop[i].offset, in.substr(begin, len + 1), prop[i].type);
                begin += (len + 3); // pass <", >
                len = 0;
            }

            // primitive: integer or floating
            else {
                // find <, > or < }>
                while (true) {
                    if((in[begin + len] == ',' && in[begin + len + 1] == ' ') ||
                        (in[begin + len] == ' ' && in[begin + len + 1] == '}')) {
                        break;
                    }
                    ++len;;
                }
                ::deserialize(out + prop[i].offset, in.substr(begin, len), prop[i].type); // ignore ',' or ' '
                begin += 3; // pass <, > or < ]>
                len = 0;
            }
        }
    }

public:
    static void deserialize(Object* out, const std::string& in) {}
};

template<> MetaClass* MetaClass::get<Object>() {
    Object object;
    return object.metaclass();
}