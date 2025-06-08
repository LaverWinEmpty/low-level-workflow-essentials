#pragma once

#include "../meta/meta.h"

/*
    serialize format

    { }: Class
    [ ]: Container
    " ": String

    e.g.
    std::string field
    { "string" }
    serialized ->
    { "\{ \"class\" \}" }
*/

#include <iostream>

// test class
namespace test {
    // test enum
    enum ETest {
        A, B, C
    };

    class Class : public lwe::Object {
        // regsiter class
        CLASS_BODY(Class, lwe::Object);

    public:
        // Currently serialize requires a default constructor.
        // I plan to add an alternate constructor in the macro.
        Class() {}
        Class(int a, int b, int c) : a(a), b(b), c(c) {}

    public:
        void print() {
            std::cout << a << ", " << b << ", " << c << "\n";
        }

    private:
        int sum() {
            return a + b + c;
        }

        void set(int inA, int inB, int inC) {
            a = inA;
            b = inB;
            c = inC;
        }

    private:
        int a, b;

    public:
        int c;
    };
};

// Register code sample
// Automation using tools is planned to be introduced.

// Negister class filelds,
// param 1: Class name
// param 2: namespace or struct, class (optional)
// e.g.
// - a::B::C  -> (C, a::B);
// - global A -> (A);
REGISTER_FIELD_BEGIN(Class, test) { // No need {}
    REGISTER_FIELD(a);
    REGISTER_FIELD(b);
    // except c
} // But if you use it, the format will be cleaner.
REGISTER_FIELD_END;

// Register method
REGISTER_METHOD_BEGIN(Class, test) {
    REGISTER_METHOD(sum);
    REGISTER_METHOD(set);
} 
REGISTER_METHOD_END;

// Register enum
REGISTER_ENUM_BEGIN(ETest, test) {
    REGISTER_ENUM(A);
    REGISTER_ENUM(B);
    REGISTER_ENUM(C);
}
REGISTER_ENUM_END;

using Test = test::Class;

// main
namespace test {

void example_reflection() {
    using namespace lwe;

    // to replace new(), use memory pool
    Test* test = create<Test>();

    // meta::Class
    // Duplicates test::Class, but is syntactically correct.
    meta::Class* metaclass = test->meta();

    // enum test
    uint64_t value = 0;

    // meta::Enum
    Enum* eval = enumof<test::ETest>();
    if (!eval) {
        std::cout << "ETest not found";
    }
    else {
        // get enum info
        const meta::Enumeration& enumReflector = eval->enums();

        // 3rd enum value
        value = enumReflector[2].value;
    }
    test->c = static_cast<int>(value);

    // class test
    // find filed
    const meta::Field& foundField = metaclass->field("c");
    if (foundField) {
        std::cout << serialize(false) << foundField.name << "found\n";
    }
    else std::cout << serialize(false) << ": c not found\n";


    // get fields
    const meta::Structure& classReflector = metaclass->fields();

    // for (auto& itr : classReflector)
    for (int i = 0; i < classReflector.size(); ++i) {
        // casting
        char* ptr = reinterpret_cast<char*>(test);

        // get filed info
        const meta::Field& f = classReflector[i];
        
        // move
        ptr += f.offset;

        // get enum
        const meta::Enumerator& e = eval->enums()[i];

        std::cout << f.name << " = " << eval->name() << "::" << e.name << "\n";

        // memcpy
        std::memcpy(ptr, &e.value, f.size); // int32 = uint64
    }

    // serialize test
    std::string str = test->serialize();
    std::cout << str << std::endl;
    
    // deserialize test
    // 2 values register
    test->deserialize("{ 100, 200 }");
    std::cout << test->serialize() << "\n";

    // method test
    meta::Method* func;
    
    // set test
    func = method<Test>("set");
    if (!func) {
        std::cout << "set not found\n";
    }
    else func->invoke(test, { 100, 200, 300 }); // call, param auto cast

    // print test
    func = method<Test>("print");
    if (!func) {
        std::cout << "print not found\n";
        test->print(); // call
    }
    else func->invoke(test, {}); // call

    // sum test
    func = method<Test>("sum");
    if (!func) {
        std::cout << "print not sum\n";
    }
    else {
        int value = func->invoke(test, {}); // call, and auto cast
        std::cout << "sum: " << value << "\n"; // 100 + 200 + 300
    }

    // check derived
    Object* obj = statics<Object>();
    std::cout << "obj =  statics<Object>();\n";

    // false
    std::cout << "obj is Test: " << serialize<bool>(obj->isof<Test>()) << "\n";


    obj = test; // set test
    std::cout << "obj = test;\n";

    // true
    std::cout << "obj is Test: " << serialize<bool>(obj->isof<Test>()) << "\n";

    // true
    std::cout << "obj is Object: " << serialize<bool>(obj->isof<Object>()) << "\n";

    // out (console)
    /*
        false: c not found
        a = ETest::A
        b = ETest::B
        { 0, 1 }
        { 100, 200 }
        print not found
        100, 200, 300
        sum: 600
        obj =  statics<Object>();
        obj is Test: false
        obj = test;
        obj is Test: true
        obj is Object: true
    */
}

}