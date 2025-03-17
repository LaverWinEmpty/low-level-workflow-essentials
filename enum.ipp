#ifdef LWE_ENUM_HEADER

template<typename E> string Enum<E>::serialize() const {
    return estring(value);
}

template<typename E> void Enum<E>::deserialize(const std::string& in) {
    value = evalue<E>(in);
}

template<typename E> Enum<E>::Enum(E in) noexcept: value(in) {}

template<typename E> Enum<E>::Enum(Underlying in) noexcept: value(static_cast<E>(in)) {}

template<typename E> Enum<E>::Enum(const Enum& in) noexcept: value(in.value) {}

template<typename E> auto Enum<E>::operator=(E in) noexcept -> Enum& {
    value = in;
    return *this;
}

template<typename E> auto Enum<E>::operator=(Underlying in) noexcept -> Enum& {
    value = static_cast<E>(in);
    return *this;
}

template<typename E> E Enum<E>::operator~() const noexcept {
    return static_cast<E>(~static_cast<Underlying>(value));
}

template<typename E> template<typename T> E Enum<E>::operator|(T in) const noexcept {
    return static_cast<E>(static_cast<Underlying>(value) | static_cast<Underlying>(static_cast<E>(in)));
}

template<typename E> template<typename T> E Enum<E>::operator&(T in) const noexcept {
    return static_cast<E>(static_cast<Underlying>(value) & static_cast<Underlying>(static_cast<E>(in)));
}

template<typename E> template<typename T> E Enum<E>::operator^(T in) const noexcept {
    return static_cast<E>(static_cast<Underlying>(value) ^ static_cast<Underlying>(static_cast<E>(in)));
}

template<typename E> template<typename T> E& Enum<E>::operator|=(T in) noexcept {
    value = value | in;
    return value;
}

template<typename E> template<typename T> E& Enum<E>::operator&=(T in) noexcept {
    value = value & in;
    return value;
}

template<typename E> template<typename T> E& Enum<E>::operator^=(T in) noexcept {
    value = value ^ in;
    return value;
}

template<typename E> template<typename T> bool Enum<E>::operator==(T in) const noexcept {
    return value == static_cast<E>(in);
}

template<typename E> template<typename T> bool Enum<E>::operator!=(T in) const noexcept {
    return !operator==(in);
}

template<typename E> template<typename T> bool Enum<E>::operator<(T in) const noexcept {
    return value < static_cast<E>(in);
}

template<typename E> template<typename T> bool Enum<E>::operator>(T in) const noexcept {
    return value > static_cast<E>(in);
}

template<typename E> template<typename T> bool Enum<E>::operator<=(T in) const noexcept {
    return value <= static_cast<E>(in);
}

template<typename E> template<typename T> bool Enum<E>::operator>=(T in) const noexcept {
    return value >= static_cast<E>(in);
}

template<typename E> template<typename T> Enum<E>::operator T() const noexcept {
    return static_cast<T>(value);
}

template<typename E> Enum<E>::operator bool() const noexcept {
    return static_cast<bool>(value);
}

#endif