#ifndef LWE_META_METHOD
#define LWE_META_METHOD

#include "registry.hpp"
#include "reflector.hpp"

LWE_BEGIN
namespace meta {

/// @brief method metadata;
/// @note  constructor, destructor, copy, move is use default
struct Signature {
    Type              ret;  //!< return type
    std::vector<Type> args; //!< argumnet types
};

/// @brief anonymous function base
class Method {
    template<typename T> friend Registered registmethod();

public:
    virtual const Signature& signature() const = 0;
    virtual ~Method() = default;
    virtual std::any invoke(void*, const std::vector<std::any>& args) const = 0;

private:
    template<typename Cls, typename Ret, typename... Args>
    static Method* lambdaize(Ret(Cls::* name)(Args...));
    template<typename Cls, typename Ret, typename... Args>
    static Method* lambdaize(Ret(Cls::* name)(Args...) const);

private:
    Signature info;
};

/// @brief a method converted to a lambda
template<typename Cls, typename Ret, typename... Args>
class Lambda : public Method {
    // tuple unpack use by index sequence
    template<size_t... Is>
    Ret call(Cls* obj, const std::vector<std::any>& args, std::index_sequence<Is...>) const;

public:
    Lambda(Ret(Cls::* name)(Args...));
    Lambda(Ret(Cls::* name)(Args...) const);
    std::any invoke(void* instance, const std::vector<std::any>& args) const override;
    const Signature& signature() const override;

private:
    union {
        Ret(Cls::* nonconst)(Args...);
        Ret(Cls::* constant)(Args...) const;
    };
    bool flag; // true == call const
};

}
LWE_END
#include "method.ipp"
#endif