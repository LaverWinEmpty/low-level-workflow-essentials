#ifdef LWE_META_LAMBDA

LWE_BEGIN
namespace meta {

Registry<Method>::~Registry() {
    for (auto& outer : table) {
        for (auto& inner : outer.second) {
            delete inner.second;
        }
    }
}

void Registry<Method>::add(const char* cls, const char* name, Method* in) {
    add(string{ cls }, string{ name }, in);
}

void Registry<Method>::add(const char* cls, const string& name, Method* in) {
    add(string{ cls }, name, in);
}

void Registry<Method>::add(const string& cls, const char* name, Method* in) {
    add(cls, string{ name }, in);
}

void Registry<Method>::add(const string& cls, const string& name, Method* lambda) {
    auto& table = instance()[cls];
    if (table.find(name) != table.end()) {
        delete lambda; // duplicate
        return;
    }
    table[name] = std::move(lambda);
}

Method* Registry<Method>::find(const char* cls, const char* name) {
    return find(string{ cls }, string{ name });
}

Method* Registry<Method>::find(const char* cls, const string& name) {
    return find(string{ cls }, name);
}

Method* Registry<Method>::find(const string& cls, const char* name) {
    return find(cls, string{ name });
}

Method* Registry<Method>::find(const string& cls, const std::string& name) {
    auto& table = instance()[cls];
    auto result = table.find(name);
    if (result != table.end()) {
        return result->second;
    }
    return nullptr;
}

auto Registry<Method>::instance() -> Table& {
    static Registry<Method> instance;
    return instance.table;
}

template<typename Cls, typename Ret, typename ...Args>
Method* Method::lambdaize(Ret(Cls::* name)(Args...)) {
    return new Lambda<Cls, Ret, Args...>(name);
}

template<typename Cls, typename Ret, typename ...Args>
Method* Method::lambdaize(Ret(Cls::* name)(Args...) const) {
    return new Lambda<Cls, Ret, Args...>(name);
}

template<typename T> Method* methodof(const char* name) {
    return methodof<T>(string{ name });
}

template<typename T> Method* methodof(const string& name) {
    // default, other class -> template specialization
    return nullptr;
}

Method* methodof(const char* cls, const char* name) {
    return Registry<Method>::find(cls, name);
}

Method* methodof(const string& cls, const char* name) {
    return Registry<Method>::find(cls, name);
}

Method* methodof(const char* cls, const string& name) {
    return Registry<Method>::find(cls, name);
}

Method* methodof(const string& cls, const string& name) {
    return Registry<Method>::find(cls, name);
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