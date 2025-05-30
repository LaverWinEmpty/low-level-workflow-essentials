#ifndef LWE_UTIL_ANY
#define LWE_UTIL_ANY

#include "../core/core.h"

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

private:
	Any(const Any&) = delete;            //!< there is no way to find the copy constructor.
	Any& operator=(const Any&) = delete; //!< there is no way to find the copy constructor.

public:
	void reset();

public:
	template<typename T> void set(T&&);

public:
	template<typename T> T cast();

public:
	template<typename T> bool check();

private:
	union {
		void* ptr = nullptr;
		// SVO
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
	};
	void (*destructor)(void*) = nullptr;

private:
	Type   type;
	size_t size;
};

}
LWE_END
#include "any.ipp"
#endif