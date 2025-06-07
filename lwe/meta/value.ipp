#ifdef LWE_META_VALUE
LWE_BEGIN
namespace meta {
    
template<typename E, typename Enable> const Enum* Value<E, Enable>::info = enumof<E>();

template<typename E, typename Enable> const Enumerator& Value<E, Enable>::meta(size_t in) {
    if (!info) {
        throw diag::Alert("NOT REGISTERED TYPE");
    }

    const Enumeration& data = info->enums();
    if (in >= data.size()) {
        throw std::out_of_range("");
    }
    return info->enums()[in];
}

template<typename E, typename Enable> const Type& Value<E, Enable>::type() {
    return typeof<E>();
}

template<typename E, typename Enable> const char* Value<E, Enable>::operator*() const noexcept {
    // not registered
    if (!info) {
        return "";
    }
    const Enumeration& data = info->enums();
    for (auto& itr : data) {
        if (itr.value == value) {
            return itr.name;
        }
    }
    return "";
}


/*
 * setter
 */

template<typename E, typename Enable> Value<E, Enable>::Value(E in) noexcept:
    value(static_cast<U>(in))
{} 

template<typename E, typename Enable> Value<E, Enable>::Value(U in) noexcept:
    value(in)
{}

template<typename E, typename Enable> auto Value<E, Enable>::operator=(const Value& in) noexcept -> Value& {
    value = in.value;
}

/*
 * getter
 */

template<typename E, typename Enable> Value<E, Enable>::operator U() const noexcept {
    return value;
}

template<typename E, typename Enable> Value<E, Enable>::operator E() const noexcept {
    return static_cast<E>(value);
}

/*
 * bit operator
 */

template<typename E, typename Enable> auto Value<E, Enable>::operator&(E in) const noexcept -> Value{
    return value & static_cast<U>(in);
}

template<typename E, typename Enable> auto Value<E, Enable>::operator|(E in) const noexcept -> Value {
    return value | static_cast<U>(in);
}

template<typename E, typename Enable> auto Value<E, Enable>::operator^(E in) const noexcept -> Value {
    return value ^ static_cast<U>(in);
}

template<typename E, typename Enable> auto Value<E, Enable>::operator&=(E in) noexcept -> Value& {
    value &= static_cast<U>(in);
    return *this;
}

template<typename E, typename Enable> auto Value<E, Enable>::operator|=(E in) noexcept -> Value& {
    value |= static_cast<U>(in);
    return *this;
}

template<typename E, typename Enable> auto Value<E, Enable>::operator^=(E in) noexcept -> Value& {
    value ^= static_cast<U>(in);
    return *this;
}

template<typename E, typename Enable> bool Value<E, Enable>::operator!() const noexcept{
    return !value;
}

template<typename E, typename Enable> auto Value<E, Enable>::operator~() const noexcept -> Value {
    return Value(~value);
}

}
LWE_END
#endif