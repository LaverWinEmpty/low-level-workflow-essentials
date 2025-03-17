#ifndef LWE_ENUM_HEADER
#define LWE_ENUM_HEADER

#include "object.hpp"

struct EInterface {
    virtual std::string serialize() const               = 0;
    virtual void        deserialize(const std::string&) = 0;
};

template<typename E> struct Enum: EInterface {
private:
    using U    = std::underlying_type_t<E>;
    using Base = Object;

public:
    friend struct MetaClass;
    struct EnumMeta: MetaClass {
        virtual const char*      name() const override { return "Enum"; }
        virtual size_t           size() const override { return sizeof(*this); }
        virtual MetaClass*       base() const override { return MetaClass::get<Object>(); }
        virtual const FieldInfo& field() const override {
            static const FieldInfo EMPTY = {
                MetaField{ EAccess::PRIVATE, typecode<U>(), "value", sizeof(0), sizeof(void*) }
            };
            return EMPTY;
        }
    };

public:
    virtual MetaClass* metaclass() const {
        static EnumMeta meta;
        return &meta;
    }

public:
    virtual std::string serialize() const override;
    virtual void        deserialize(const std::string&) override;

public:
    Enum() noexcept       = default;
    Enum(Enum&&) noexcept = default;
    ~Enum()               = default;

public:
    Enum(E) noexcept;
    Enum(U) noexcept;
    Enum(const Enum&) noexcept;

public:
    Enum& operator=(E) noexcept;
    Enum& operator=(U) noexcept;

public:
    template<typename T> E operator|(T) const noexcept;
    template<typename T> E operator&(T) const noexcept;
    template<typename T> E operator^(T) const noexcept;

public:
    template<typename T> E& operator|=(T) noexcept;
    template<typename T> E& operator&=(T) noexcept;
    template<typename T> E& operator^=(T) noexcept;

public:
    E operator~() const noexcept;

public:
    template<typename T> bool operator==(T) const noexcept;
    template<typename T> bool operator!=(T) const noexcept;
    template<typename T> bool operator<(T) const noexcept;
    template<typename T> bool operator>(T) const noexcept;
    template<typename T> bool operator<=(T) const noexcept;
    template<typename T> bool operator>=(T) const noexcept;

public:
    template<typename T> explicit operator T() const noexcept;
    explicit                      operator bool() const noexcept;

private:
    E           value;
    const char* name;
};

#endif