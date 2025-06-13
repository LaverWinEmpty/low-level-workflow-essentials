#pragma once

#include "iostream"
#include "../meta/meta.h"

/*
 * class reflection -> example_object.hpp
 */

namespace test {

enum class EnumTest : uint8_t {
	FALSE, TRUE, WHAT
};

enum class EnumUnregistered : uint8_t {
	A, B
};

class MethodTest {
// private possible: Object derived class only
public:
	void print() const { std::cout << "Hello, world!\n"; }
	int  sum(int a, int b) const { return a + b; }
	int  sub(int a, int b) const { return a - b; }
};

using namespace LWE;
using namespace meta;
}

/* -Parameters: (ClassName, namespace) or (ClassName) for global namespace
 * -Limitation : Method overloading is NOT SUPPORTED
 * -Note : Registration macros must be used in global namespace only
 */
REGISTER_METHOD_BEGIN(MethodTest, test) {
	REGISTER_METHOD(print);
	REGISTER_METHOD(sum);
}
REGISTER_METHOD_END;
// enum
REGISTER_ENUM_BEGIN(EnumTest, test) {
	REGISTER_ENUM(FALSE);
	REGISTER_ENUM(TRUE);
}
REGISTER_ENUM_END;

namespace test {

void example_reflection() {
	/* Serialize format
	 * string:    " "
	 * container: [ ]
	 * class:     { }
	 * need "object.hpp"
	 */

	/* Type info class (without RTTI) */ {
		int variable;

		Type a = typeof<int>();    // by template
		Type b = typeof(variable); // by variable

		// * operator returns const char
		printf("%s\n", *a); // print int
		printf("%s\n", *b); // print int

		Type c = typeof<Test>(); // registered class
		Type d = typeof<Temp>(); // unregistered class

		printf("%s\n", *c); // print Test
		printf("%s\n", *d); // print class

		// composite type
		Type e = typeof<const void**>();
		Type f = typeof<void** const>();
		Type g = typeof<const void** const>();
		printf("%s\n", *e); // print const void**
		printf("%s\n", *f); // print void** const
		printf("%s\n", *g); // print const void** const
	}

	/* get enum info */ {
		Enum* info = nullptr;

		// get by template
		info = enumof<EnumTest>();
		std::cout << info->name() << "\n"; // print name

		EnumTest e;
		info = enumof(e);                     // get by variable
 		const Enumeration& a = info->enums(); // get enum values
		std::cout << a[0].name << " ";        // print [0] enum name
		
		info = enumof("EnumTest");            // get by name
		const Enumeration& b = info->enums(); // get enum values
		std::cout << b[0].value << "\n";      // print [0] enum value

		// unregistered == nullptr
		info = enumof<EnumUnregistered>();
		if (!info) {
			printf("INFO IS NULL\n");
		}
	}

	/* enum helper */ {
		Value<EnumTest> value = 65; // Acts like an integer, but restricted to EnumTest values
		std::cout << "value: " << value << "\n"; // print 'A'
		std::cout << "value TO STRING: " << *value << "\n"; // to string (unregistered value = empty)

		value = EnumTest::TRUE;
		std::cout << "value TO STRING: " << *value << "\n"; // print TRUE (Only registered value are allowed)

		// Bit operation supported
		std::cout << "value & TRUE == " << (int)(value & EnumTest::TRUE) << "\n"; // print 1

		Value<EnumUnregistered> other = 100;
		std::cout << "other TO STRING " << *other << "\n"; // unregistered enum -> empty string

		// get value info (unregistered -> error)
		diag::Expected<Enumerator>info = other.meta();
		if (!info) {
			std::cout << info.what() << std::endl;
		}

		// get value info (unregistered -> error)
		info = Value<EnumTest>::find(2);
		if (!info) {
			std::cout << info.what() << std::endl;
		}

		// get value info by index (out of range)
		info = Value<EnumTest>::get(2);
		if (!info) {
			std::cout << info.what() << std::endl;
		}

		// get enum info by name
		info = Value<EnumTest>::find("TRUE");
		if (!info) {
			std::cout << info.what() << std::endl;
		}
		else std::cout << (*info).name << ", " << info->value << "\n"; // print TRUE, 1
	}

	/* method */ {
		Method* m;

		// Get by template + method name
		m = methodof<MethodTest>("print");

		// Param is std::vector<util::Any>, `Any` is RTTI-free custom any
		m->invoke(nullptr, {});

		// Get by name + method name
		m = methodof("MethodTest", "sum");

		// Return `util::Any`
		util::Any any = m->invoke(nullptr, { 1 , 2 });

		// Auto inference
		int value = any;
		std::cout << value << "\n";

		// Or cast
		std::cout << any.cast<int>() << "\n";

		// Exist, but unregistered method
		m = methodof<Method>("sub");
		if (!m) {
			printf("METHOD IS NULL\n");
		}
	}
}

}