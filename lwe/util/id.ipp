#ifdef LWE_UTIL_ID

LWE_BEGIN
namespace util {

template<typename T> uint64_t ID<T>::next() {
    static std::atomic<uint64_t> id = 1;
    return id.fetch_add(1, std::memory_order_relaxed);
}

template<typename T> ID<T>::ID(Uninit): id(0) {}

template<typename T> ID<T>::ID(): id(next()) {}

template<typename T> bool ID<T>::operator==(const ID& in) const {
    return id == in.id;
}

template<typename T> bool ID<T>::operator!=(const ID& in) const {
    return id != in.id;
}

template<typename T> ID<T>::operator uint64_t() const {
    return id;
}

template<typename T> uint64_t ID<T>::value() const {
    return id;
}

template<typename T> void ID<T>::gen() {
    if(!id) {
        id = next();
    }
}

}
LWE_END
#endif