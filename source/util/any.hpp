#ifndef LWE_UTIL_ANY
#define LWE_UTIL_ANY

#include "../meta/type.hpp"

LWE_BEGIN
namespace util {

class Any {
public:
	Any();

public:
	template<typename T> Any(T&&);

public:
	~Any();

public:
	Any(Any&& in) noexcept;

public:
	Any& operator=(Any&& in) noexcept;

public:
	Any(const Any&);

public:
	Any& operator=(const Any&);

public:
	void reset();

public:
	template<typename T> void set(T&&);

public:
	template<typename T> T cast(bool = false) const;

public:
	template<typename T> bool check() const;

public:
	const meta::Type& type() const;

public:
	explicit operator bool() const noexcept;

private:
	union {
		void*              ptr;
		char               c;
		signed char        sc;
		signed int         si;
		signed long        sl;
		signed short       ss;
		signed long long   sll;
		unsigned char      uc;
		unsigned int       ui;
		unsigned long      ul;
		unsigned short     us;
		unsigned long long ull;
		bool               b;
		float              f;
		double             d;
		long double        ld;
	} data = { 0 };
	void (*deleter)(void*) = nullptr;
	void (*copier)(void*, void*) = nullptr;

private:
	meta::Type info;
	size_t     size;
};

}
LWE_END
#include "any.ipp"
#endif