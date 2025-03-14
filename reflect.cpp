#include "meta.hpp"


class TestA {
    double d;
    char test[83];
    int v;
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
            itr->offset = offset;                            // set
            offset += itr->size;                             // next
            result.emplace_back(*itr++);                     // add
        }
        while(itr != end) {
            // 1 바이트 공간에 [7] current
            // 4바이트 넣으려면 [8] next

            // align 7 -> 8
            // 근데 1바이트 넣을거면 그대로 1
            // alignof(T) 보다 작은 값은
            // align(offset, itr->size)
            // 근데 크면 
            offset = itr->size <= alignof(T) ? lwe::Common::align(offset, itr->size) : lwe::Common::align(offset, alignof(T));
            itr->offset = offset;        // set
            offset += itr->size;         // next
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

        char* ptr = const_cast<char*>(reinterpret_cast<const char*>(this));
        for(int i = 0; i < prop.size(); ++i) {
            ::serialize(&buffer, ptr + prop[i].offset, prop[i].type);
        }
        return buffer;
    }

public:
    static void deserialize(Object* out, const std::string& in) {}
};

template<> MetaClass* MetaClass::get<Object>() {
    Object object;
    return object.metaclass();
}