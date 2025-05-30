#ifdef LWE_UTIL_ANY

#include "../meta/meta.h"

LWE_BEGIN
namespace util {

Any::Any() {
	reset();
}

template<typename T> Any::Any(T&& in) {
	set(std::forward<T>(in));
}

Any::~Any() {
	reset();
}

Any::Any(Any&& in) noexcept: ptr(in.ptr), destructor(in.destructor), size(in.size) {
	type = std::move(in.type); // for allocation optimization;
	in.reset();
}

Any& Any::operator=(Any && in) noexcept {
	if (this != &in) {
		ptr = in.ptr;
		destructor = in.destructor;
		size = in.size;
		type = std::move(in.type);
		in.reset();
	}
	return *this;
}

void Any::reset() {
	if (ptr && destructor) {
		destructor(ptr);
	}
	ptr = nullptr; // required for check
	destructor = nullptr; // not required, but for safety
	size = 0;       // not required, but for safety
	type = {};      // not required, but for safety
}

template<typename T> void Any::set(T&& in) {
	using Typename = std::remove_reference_t<T>;

	// reset
	if (ptr && destructor) {
		destructor(ptr);
	}

	type = typeof<Typename>(); // store
	size = sizeof(Typename);   // store

	// primitive, enum, pointer types do not call destructor
	// pointer: not sure if it's allocated
	if constexpr (std::is_fundamental_v<Typename> ||
		std::is_enum_v<Typename> ||
		std::is_pointer_v<Typename>) {
		destructor = nullptr;
	}
	// else type
	else destructor =
		[](void* in) {
		if constexpr (!std::is_pointer_v<Typename>) {
			delete static_cast<Typename*>(in);
		}
		};

	// primitive, enum, pointer types use SVO
	if constexpr (std::is_fundamental_v<Typename> ||
		std::is_enum_v<Typename> ||
		std::is_pointer_v<Typename>) {
		ptr = reinterpret_cast<void*>(in); // insert to union
	}
	// else copy or move constructor
	else ptr = new Typename(std::forward<T>(in));
}

template<typename T> T Any::cast() {
	// clang-format off
	// pointer
	if constexpr (std::is_pointer_v<T>) {
		if (type == meta::Keyword::POINTER) {
			return static_cast<T>(ptr);
		}
	}
	// primitive
	else {
		if (type == meta::Keyword::CHAR)               return static_cast<T>(c);
		if (type == meta::Keyword::SIGNED_CHAR)        return static_cast<T>(sc);
		if (type == meta::Keyword::SIGNED_INT)         return static_cast<T>(si);
		if (type == meta::Keyword::SIGNED_LONG)        return static_cast<T>(sl);
		if (type == meta::Keyword::SIGNED_SHORT)       return static_cast<T>(ss);
		if (type == meta::Keyword::SIGNED_LONG_LONG)   return static_cast<T>(sll);
		if (type == meta::Keyword::UNSIGNED_CHAR)      return static_cast<T>(uc);
		if (type == meta::Keyword::UNSIGNED_INT)       return static_cast<T>(ui);
		if (type == meta::Keyword::UNSIGNED_LONG)      return static_cast<T>(ul);
		if (type == meta::Keyword::UNSIGNED_SHORT)     return static_cast<T>(us);
		if (type == meta::Keyword::UNSIGNED_LONG_LONG) return static_cast<T>(ull);
		if (type == meta::Keyword::BOOL)               return static_cast<T>(b);
		if (type == meta::Keyword::FLOAT)              return static_cast<T>(f);
		if (type == meta::Keyword::DOUBLE)             return static_cast<T>(d);
		if (type == meta::Keyword::LONG_DOUBLE)        return static_cast<T>(ld);
		if (type == meta::Keyword::ENUM)               return static_cast<T>(ull); // uint64 -> enum (cut)
	}
	// clang-format on

	// other: compare only size
	// use check<T>() for safety
	if (sizeof(T) > size) {
		throw std::bad_any_cast();
	}
	return *static_cast<T*>(ptr);
}

template<typename T> bool Any::check() {
	return type.hash() == typeof<T>().hash();
}

}
LWE_END
#endif