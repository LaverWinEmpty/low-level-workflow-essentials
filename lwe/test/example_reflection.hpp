#pragma once

#include "../meta/meta.h"

#include "iostream"


// SAMPLE
namespace test {
struct ReflTest: public LWE::meta::Object {
    CLASS_BODY(Reflect, Object);

public:
    void method() { std::cout << b << std::endl; }

public:
    int& getA() { return a; }

private:
    int a;
    int b = 999;
};

enum EnumTest : uint64_t {
    A,
    B,
    C,
};
} // namespace test

/* === QUICK START === */

// REGISTER FIELD
REGISTER_FIELD_BEGIN(ReflTest, test) {
    REGISTER_FIELD(a);
    // REGISTER_FIELD(a); ... PASS
}
REGISTER_FIELD_END

// REGISTER METHOD
REGISTER_METHOD_BEGIN(ReflTest, test) {
    REGISTER_METHOD(method);
}
REGISTER_METHOD_END

// REGISTER ENUM
REGISTER_ENUM_BEGIN(EnumTest, test) {
    REGISTER_ENUM(A);
    REGISTER_ENUM(B);
}
REGISTER_ENUM_END;

namespace test {

using namespace lwe;
using namespace lwe::meta;

void example_reflection() {
    ReflTest test;

    // GET TYPE API
    const meta::Type& type = typeof<ReflTest>();

    // GET META ENUM API
    meta::Enum* e = enumof<EnumTest>();

    // GET META CLASS API
    meta::Class* cls = classof<ReflTest>(); // or test.meta(); (virtual)

    // GET METHOD API
    auto method = methodof<ReflTest>("method");
    method->invoke(&test, {}); // CALL

    /* === DETAIL === */

    // TYPE METHODS
    std::cout << std::endl;
    std::cout << (uint)type.code();             // get main type Keyword (const int* const -> POINTER)
    std::cout << type.hash() << std::endl;      // get hash
    std::cout << type.stringify() << std::endl; // type name to string
    std::cout << *type << std::endl;            // type name to string

    // Internal API ---------------+
    type.begin(); // keyword begin |
    type.end();   // keyword end   |
    type.count(); // keyword count |
    //-----------------------------+

    // OBJECT METHODS
    meta::Object* obj = nullptr;
    std::cout << obj->isof<ReflTest>() << "\n"; // obj (is / base of) ReflTest? (nullptr == false)
    std::cout << test.isof<Object>() << "\n";   // test (is / base of) Object? (true)

    // OBJECT SERIALIZE
    test.getA() = 100;
    std::cout << test.serialize() << "\n"; // only variable `a`
    test.deserialize("{ 200 }");           // deserialize test
    std::cout << test.getA() << "\n";      // output

    // META CLASS METHODS
    cls->base();           // get meta class of ReflTest::Base (Object)
    cls->name();           // get class name (virtual)
    cls->size();           // get class size (virtual)
    cls->construct(&test); // Call default constructor (internal)

    const meta::Structure& f = cls->fields(); // get filed list
    f.begin();                                // get begin
    f.end();                                  // get end
    f.size();                                 // get parameter count
    f[0];                                     // get parameter

    // f.find is...
    const meta::Structure& f0 = meta::Structure::find<ReflTest>(); // find
    const meta::Structure& f1 = meta::Structure::find("ReflTest"); // find
    const meta::Structure& f2 = meta::Structure::find(*cls);       // find

    std::cout << std::endl;
    const meta::Field& a = cls->field("a"); // find field, failed == empty
    if(a) {
        std::cout << "name:   " << a.name << "\n";   // name
        std::cout << "offset: " << a.offset << "\n"; // offset
        std::cout << "size:   " << a.size << "\n";   // size
        std::cout << "type:   " << *a.type << "\n";  // type info
    }

    std::cout << std::endl;
    // META ENUM METHODS
    std::cout << e->name() << "\n";              // get enum name
    std::cout << e->size() << "\n";              // get enum size
    const meta::Enumeration& enums = e->enums(); // like as Class::fields

    // enum serialize / deserialize API -> WIP

    // ENUM HELPER
    Value<EnumTest> eVal = 10; // ENUM ONLY, unregistered enum allowed, but function is error

    diag::Expected<Enumerator> except_0 = Value<EnumTest>::at(0);     // get by index
    diag::Expected<Enumerator> except_1 = Value<EnumTest>::find("B"); // find by string
    diag::Expected<Enumerator> except_2 = Value<EnumTest>::find(C);   // find by value

    // error is not thorw
    if(except_0) std::cout << except_0->name << "\n"; 
    if(except_1) std::cout << except_1->name << "\n"; 
    if(except_2) std::cout << except_2->name << "\n"; // unregistered

    Enumerator enumerator = eVal.info(); // get except skip
    enumerator.name;                     // get name
    enumerator.value;                    // get value

    // SMART POINTER
    RC<ReflTest> rc;
    RC<ReflTest> weak = rc; // IS SAME AS `Ptr<T>` (example_ptr)

    weak.clone(); // copy
    weak.valid(); // check nullptr / dangling
    weak.owned(); // check owner
    weak.own();   // set owner

    // safe casting
    if (weak.as<Object>()) {
        std::cout << weak.as<Object>()->meta()->name() << "\n";
    }
}

} // namespace test
