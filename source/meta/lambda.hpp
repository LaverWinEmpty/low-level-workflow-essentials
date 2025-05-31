#ifndef LWE_META_LAMBDA
#define LWE_META_LAMBDA

#include "type.hpp" // use Signature { Type ... }
#include "internal/feature.hpp"

#include "../util/any.hpp"
#include "../diag/alert.hpp"

LWE_BEGIN
namespace meta {

/// @brief anonymous function base
class Method {
   template<typename T> friend Registered registmethod();

public:
   virtual const Signature& signature() const = 0;
   virtual ~Method() = default;
   virtual util::Any invoke(void*, const std::vector<util::Any>& args) const = 0;

public:
   template<typename Cls, typename Ret, typename... Args> static Method* lambdaize(Ret(Cls::* name)(Args...));
   template<typename Cls, typename Ret, typename... Args> static Method* lambdaize(Ret(Cls::* name)(Args...) const);

private:
   Signature info;
};

/// @brief a method converted to a lambda
template<typename Cls, typename Ret, typename... Args>
class Lambda : public Method {
   // tuple unpack use by index sequence
   template<size_t... Is>
   Ret call(Cls* obj, const std::vector<util::Any>& args, std::index_sequence<Is...>) const;

public:
   Lambda(Ret(Cls::* name)(Args...));
   Lambda(Ret(Cls::* name)(Args...) const);
   util::Any invoke(void* instance, const std::vector<util::Any>& args) const override;
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
#include "lambda.ipp"
#endif