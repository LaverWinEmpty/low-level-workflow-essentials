LWE_BEGIN
namespace meta {

template<typename E, typename Enable> const Enum* Value<E, Enable>::cache = enumof<E>();

// find by value
template<typename E, typename Enable> diag::Expected<Enumerator> Value<E, Enable>::find(uint64_t in) {
    if(!cache) {
        return diag::error(diag::Code::TYPE_MISMATCH);
    }
    const Enumeration& data = cache->enums();
    // find same value
    for(auto& e : data) {
        if(e.value == in) {
            return e; // found
        }
    }
    return diag::error(diag::Code::INVALID_DATA); // not found
}

// find by name
template<typename E, typename Enable> diag::Expected<Enumerator> Value<E, Enable>::find(const StringView in) {
    if(!cache) {
        return diag::error(diag::Code::TYPE_MISMATCH);
    }
    const Enumeration& data = cache->enums();
    // find same value
    for(auto& e : data) {
        if(strcmp(e.name, in) == 0) {
            return e; // found
        }
    }
    return diag::error(diag::Code::INVALID_DATA); // not found
}

// get by index
template<typename E, typename Enable> diag::Expected<Enumerator> Value<E, Enable>::at(size_t in) {
    if(!cache) {
        return diag::error(diag::Code::TYPE_MISMATCH);
    }
    const Enumeration& data = cache->enums();
    if(in >= data.size()) {
        return diag::error(diag::Code::OUT_OF_RANGE);
    }
    return cache->enums()[in];
}

template<typename E, typename Enable> diag::Expected<Enumerator> Value<E, Enable>::info() {
    return find(value);
}

template<typename E, typename Enable> const Type& Value<E, Enable>::type() {
    return typeof<E>();
}

template<typename E, typename Enable> const char* Value<E, Enable>::operator*() const noexcept {
    // not registered
    if(!cache) {
        return "";
    }
    const Enumeration& data = cache->enums();
    for(auto& itr : data) {
        if(itr.value == value) {
            return itr.name;
        }
    }
    return "";
}

/*
 * setter
 */

template<typename E, typename Enable> Value<E, Enable>::Value(E in) noexcept: value(static_cast<U>(in)) { }

template<typename E, typename Enable> Value<E, Enable>::Value(U in) noexcept: value(in) { }

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

template<typename E, typename Enable> auto Value<E, Enable>::operator&(E in) const noexcept -> Value {
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

template<typename E, typename Enable> bool Value<E, Enable>::operator!() const noexcept {
    return !value;
}

template<typename E, typename Enable> auto Value<E, Enable>::operator~() const noexcept -> Value {
    return Value(~value);
}

} // namespace meta
LWE_END
