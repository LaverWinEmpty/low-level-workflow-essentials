#ifdef LWE_UTIL_ANY

#include "../diag/alert.hpp"

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

Any::Any(Any&& in) noexcept: data(in.data), deleter(in.deleter), copier(in.copier), size(in.size) {
	info = std::move(in.info); // for allocation optimization;
	in.reset();
}

Any::Any(const Any& in): deleter(in.deleter), copier(in.copier), size(in.size), info(in.info) {
	// primitive info (SVO)
	if (!copier) {
		data = in.data; // shallow copy
	}
	else {
		data.ptr = malloc(size); // malloc
		if (!data.ptr) {
			reset();
			throw std::bad_alloc();
		}
		copier(data.ptr, in.data.ptr); // placement new
	}
}

Any& Any::operator=(const Any& in){
	if (data.ptr && deleter) {
		deleter(data.ptr); // call destructor
	}

	// other is null
	if (!in.data.ptr) {
		reset();
	}

	// else copy
	else {
		// copy
		copier  = in.copier;
		deleter = in.deleter;
		info    = in.info;
		size    = in.size;

		// primitive type (SVO)
		if (!copier) {
			data = data; // shallow copy
		}
		// else call copy constructor
		else {
			data.ptr = malloc(size); // malloc
			if (!data.ptr) {
				reset();
				throw std::bad_alloc();
			}
			copier(data.ptr, in.data.ptr); // placement new
		}
	}

	return *this;
}

Any& Any::operator=(Any && in) noexcept {
	if (this != &in) {
		data.ptr = in.data.ptr;
		deleter = in.deleter;
		size = in.size;
		info = std::move(in.info);
		in.reset();
	}
	return *this;
}

void Any::reset() {
	if (data.ptr && deleter) {
		deleter(data.ptr);
		deleter = nullptr; // init
		copier  = nullptr; // init
	}
	data.ptr = nullptr; // required for check
}

template<typename T> void Any::set(T&& in) {
	using Typename = std::remove_reference_t<T>;

	// reset
	if (data.ptr && deleter) {
		deleter(data.ptr);
	}

	info = meta::typeof<Typename>(); // store
	size = sizeof(Typename);         // store

	// primitive, enum, pointer types do not call destructor
	// pointer: not sure if it's allocated
	if constexpr (std::is_fundamental_v<Typename> ||
		std::is_enum_v<Typename> ||
		std::is_pointer_v<Typename>) {
		deleter = nullptr;
		copier  = nullptr;
	}
	// else type
	else {
		deleter = [](void* in) {
			delete static_cast<Typename*>(in);
		};
		copier = [](void* dsc, void* src) {
			new (dsc) Typename(*static_cast<Typename*>(src));
		};
	}

	// primitive, enum, pointer types use SVO (union)
	// note
	// - new __int128  -> size exceeds max
	// - new std::byte -> has deconstructor
	//
	// clang-format off 
	if      constexpr (std::is_same_v<T, char>)               { data.c   = in; }
	else if constexpr (std::is_same_v<T, signed char>)        { data.sc = in; }
	else if constexpr (std::is_same_v<T, signed int>)         { data.si = in; }
	else if constexpr (std::is_same_v<T, signed long>)        { data.sl = in; }
	else if constexpr (std::is_same_v<T, signed short>)       { data.ss = in; }
	else if constexpr (std::is_same_v<T, signed long long>)   { data.sll = in; }
	else if constexpr (std::is_same_v<T, unsigned char>)      { data.uc = in; }
	else if constexpr (std::is_same_v<T, unsigned int>)       { data.ui = in; }
	else if constexpr (std::is_same_v<T, unsigned long>)      { data.ul = in; }
	else if constexpr (std::is_same_v<T, unsigned short>)     { data.us = in; }
	else if constexpr (std::is_same_v<T, unsigned long long>) { data.ull = in; }
	else if constexpr (std::is_same_v<T, bool>)               { data.b = in; }
	else if constexpr (std::is_same_v<T, float>)              { data.f = in; }
	else if constexpr (std::is_same_v<T, double>)             { data.d = in; }
	else if constexpr (std::is_same_v<T, long double>)        { data.ld = in; }
	else if constexpr (std::is_enum_v<T>)                     { data.ull = static_cast<uint64_t>(in); } // expand
	else if constexpr (std::is_pointer_v<T>)                  { data.ptr = reinterpret_cast<void*>(in); }
	// clang-format on
	// else copy or move constructor
	else { data.ptr = new Typename(std::forward<T>(in)); }
}

template<typename T> T Any::cast(bool safety) const {
	if (std::is_reference_v<T>) {
		throw diag::Alert("BAD ANY CAST");
	}
	if (safety) {
		if (!check<T>()) {
			throw diag::Alert("BAD ANY CAST");
		}
	}

	// clang-format off
	// pointer
	if constexpr (std::is_pointer_v<T>) {
		if (info == meta::Keyword::POINTER) {
			return static_cast<T>(data.ptr);
		}
	}
	// primitive
	else {
		if (info == meta::Keyword::CHAR)               return static_cast<T>(data.c);
		if (info == meta::Keyword::SIGNED_CHAR)        return static_cast<T>(data.sc);
		if (info == meta::Keyword::SIGNED_INT)         return static_cast<T>(data.si);
		if (info == meta::Keyword::SIGNED_LONG)        return static_cast<T>(data.sl);
		if (info == meta::Keyword::SIGNED_SHORT)       return static_cast<T>(data.ss);
		if (info == meta::Keyword::SIGNED_LONG_LONG)   return static_cast<T>(data.sll);
		if (info == meta::Keyword::UNSIGNED_CHAR)      return static_cast<T>(data.uc);
		if (info == meta::Keyword::UNSIGNED_INT)       return static_cast<T>(data.ui);
		if (info == meta::Keyword::UNSIGNED_LONG)      return static_cast<T>(data.ul);
		if (info == meta::Keyword::UNSIGNED_SHORT)     return static_cast<T>(data.us);
		if (info == meta::Keyword::UNSIGNED_LONG_LONG) return static_cast<T>(data.ull);
		if (info == meta::Keyword::BOOL)               return static_cast<T>(data.b);
		if (info == meta::Keyword::FLOAT)              return static_cast<T>(data.f);
		if (info == meta::Keyword::DOUBLE)             return static_cast<T>(data.d);
		if (info == meta::Keyword::LONG_DOUBLE)        return static_cast<T>(data.ld);
		if (info == meta::Keyword::ENUM)               return static_cast<T>(data.ull); // uint64 -> enum (cut)
	}
	// clang-format on

	// other: compare only size
	// use check<T>() for safety
	if (sizeof(T) > size) {
		throw diag::Alert("BAD ANY CAST");
	}
	return *static_cast<T*>(data.ptr);
}

template<typename T> bool Any::check() const {
	return info.hash() == meta::typeof<T>().hash();
}

const meta::Type& Any::type() const {
	return info;
}

Any::operator bool() const noexcept {
	return data.ptr != nullptr;
}

}
LWE_END
#endif