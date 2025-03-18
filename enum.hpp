#ifndef LWE_ENUM_HEADER
#define LWE_ENUM_HEADER

#include "meta.hpp"

template<typename E> struct Enum: EInterface {
private:
    using Underlying = std::underlying_type_t<E>;
    using Base       = EInterface;

public:
    friend struct MetaClass;
    struct EnumMeta: MetaClass {
        const char*      name() const override { return "Enum"; }
        size_t           size() const override { return sizeof(*this); }
        MetaClass*       base() const override { return nullptr; }
        const FieldInfo& fields() const override {
            static const FieldInfo VECTOR = {
                MetaField{ typecode<Underlying>(), "value", sizeof(0), sizeof(void*) }
            };
            return VECTOR;
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
    Enum(Underlying) noexcept;
    Enum(const Enum&) noexcept;

public:
    Enum& operator=(E) noexcept;
    Enum& operator=(Underlying) noexcept;

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