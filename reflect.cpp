#include "meta.hpp"

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
            result.reserve(loop + parent->properties().size()); // for append
        } else result.reserve(loop);

        // declare append lambda
        std::function<void(std::vector<MetaField>&, const MetaClass*)> append = [&append](std::vector<MetaField>& out,
                                                                                          const MetaClass* base) {
            if(!base) {
                return; // end
            }
            append(out, base->base()); // parent first
            for(auto& itr : base->properties()) {
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
        const MetaField* end = list.end();

        while(itr != end) {
            size_t align = itr->size >= alignof(T) ? alignof(T) : itr->size; // max: alignof(T)
            result.emplace_back(*itr);
            offset += itr->size;
            ++itr;
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
        virtual const FieldInfo& properties() const override {
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
        const FieldInfo& prop = metaclass()->properties();

        std::string buffer;
        buffer.reserve(4096);

        for(int i = 0; i < prop.size(); ++i) {}
    }

public:
    static void deserialize(Object* out, const std::string& in) {}
};