#ifndef LWE_REFLECT_HEADER
#define LWE_REFLECT_HEADER

#include "meta.hpp"
#include "serialize.hpp"

//! @brief
template<typename T> FieldInfo reflect(std::initializer_list<MetaField> list);

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
    virtual MetaClass* metaclass() const {
        static ObjectMeta meta;
        return &meta;
    }
    virtual MetaClass* metabase() { return nullptr; }

public:
    std::string serialize() const;
    void        deserialize(const std::string& in);
    static void deserialize(Object* out, const std::string& in);
};

#include "reflect.ipp"
#endif