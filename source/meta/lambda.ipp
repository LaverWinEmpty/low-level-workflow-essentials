#ifdef LWE_META_LAMBDA

LWE_BEGIN
namespace meta {

template<typename Cls, typename Ret, typename ...Args>
Method* Method::lambdaize(Ret(Cls::* name)(Args...)) {
    return new Lambda<Cls, Ret, Args...>(name);
}

template<typename Cls, typename Ret, typename ...Args>
Method* Method::lambdaize(Ret(Cls::* name)(Args...) const) {
    return new Lambda<Cls, Ret, Args...>(name);
}

template<typename Cls, typename Ret, typename ...Args>
template<size_t ...N>
Ret Lambda<Cls, Ret, Args...>::call(Cls* obj, const std::vector<util::Any>& args, std::index_sequence<N...>) const {
    if (flag) {
        return (obj->*constant)(args[N].cast<typename std::decay<Args>::type>()...);
    }
    else return (obj->*nonconst)(args[N].cast<typename std::decay<Args>::type>()...);
}

template<typename Cls, typename Ret, typename ...Args>
Lambda<Cls, Ret, Args...>::Lambda(Ret(Cls::* name)(Args...)) : nonconst(name), flag(false) {}

template<typename Cls, typename Ret, typename ...Args>
Lambda<Cls, Ret, Args...>::Lambda(Ret(Cls::* name)(Args...) const) : constant(name), flag(true) {}

template<typename Cls, typename Ret, typename... Args>
util::Any Lambda<Cls, Ret, Args...>::invoke(void* instance, const std::vector<util::Any>& args) const {
    if (args.size() != sizeof...(Args)) {
        throw diag::Alert("PARAMITER COUNT MISMATCH.");
    }

    Cls* obj = static_cast<Cls*>(instance);
    if constexpr (std::is_void_v<Ret>) {
        call(obj, args, std::index_sequence_for<Args...>{});
        return util::Any(); // if return type is void, an empty value is returned.
    }
    else {
        return call(obj, args, std::index_sequence_for<Args...>{});
    }
}

template<typename Class, typename Ret, typename ...Args>
const Signature& Lambda<Class, Ret, Args...>::signature() const {
    static const Signature info = {
        typeof<Ret>(),
        std::vector<Type>{ typeof<typename std::decay<Args>::type>()... }
    };
    return info;
}

}
LWE_END
#endif