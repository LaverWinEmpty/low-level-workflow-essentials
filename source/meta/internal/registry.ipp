#ifdef LWE_META_REGISTRY

#include "method.hpp"

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

template<typename T> T* Registry<T>::find(const char* in) {
    return find(string{ in });
}

template<typename T> T* Registry<T>::find(const string& in) {
    auto result = instance().find(in);
    if(result != instance().end()) {
        return result->second;
    }
    return nullptr;
}

template<typename T> template<typename U> void Registry<T>::add(const char* in) {
    add<U>(string{ in });
}

template<typename T> template<typename U> void Registry<T>::add(const string& in) {
    Table& table = instance();
    if (table.find(in) == table.end()) {
        table.insert({ in, static_cast<T*>(new U()) });
    }
}

template<typename T> Registry<T>::~Registry() {
    for(auto& it : table) {
        delete it.second;
    }
}

template<typename T> auto Registry<T>::instance()->Table& {
    static Registry<T> statics;
    return statics.table;
}

}
LWE_END
#endif