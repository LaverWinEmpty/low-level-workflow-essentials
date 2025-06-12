#pragma once

namespace test {
    using namespace LWE;
    using namespace meta;
    using namespace stl;

// unregistered class
class Temp {};

// registered class
class Test : public Object {
    CLASS_BODY(Test, Object);

public:
    int value = 0;

public:
    Deque<string> deque;
};
}

// Register global namespace
// namespace is optional (::Test => REGISTER_FIELD(Test))
// {} is optional
REGISTER_FIELD_BEGIN(Test, test) {
    REGISTER_FIELD(deque); // private OK
}
REGISTER_FIELD_END;

namespace test{
int example_object() {
    /* Serialize format */
    // string:    " "
    // container: [ ]
    // class:     { }

    /* Type info class (with out RTTI) */ {
        int variable;

        Type a = typeof<int>();    // by template
        Type b = typeof(variable); // by variable

        // *type -> get const char*
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

    /* RC (Reflectable Class)*/ {
        // This class behaves exactly like Ptr: unique for construction and weak for copying.
        RC<Test> temp;        // create with instance (Required: default constructor)
        RC<Test> weak = temp; // weak_ptr
        temp = nullptr;       // delete
        if (!weak) {
            std::cout << "DANGLING POINTER\n"; // enter this line
        }
    }

    /* Construct */ {
        RC<Test> test;
        std::cout << "Test::value default: " << test->value << "\n"; // print 0

        test = nullptr; // delete
        
        statics<Test>()->value = 100; // set default class

        test = RC<Test>(); // create
        std::cout << "Test::value default: " << test->value << "\n"; // print 100
    }

    /* Casting */ {
        Object* obj  = nullptr;
        Test*   test = nullptr;

        std::cout << "test is of Object:\t" << test->isof<Object>() << "\n"; // nullptr: failed

        // not use memory pool, and ignore default class setting
        obj  = new Object();
        test = new Test();

        // isof: up/down cast check method
        std::cout << "obj is of Test:\t\t"  << obj->isof<Test>()    << "\n"; // false
        std::cout << "obj is of Object:\t"  << obj->isof<Object>()  << "\n"; // true
        std::cout << "test is of Test:\t"   << test->isof<Test>()   << "\n"; // true
        std::cout << "test is of Object:\t" << test->isof<Object>() << "\n"; // true
    }

    /* class info */ {
        RC<Test> temp;

        // Get meta class
        meta::Class* metaClass = temp->meta(); // meta Class
        
        // Get base class meta class
        std::cout << "Test Base: " << metaClass->base()->name() << "\n"; // print Object

        // Find not registered variable
        const Field& field = metaClass->field("value"); // copyable
        if(field) {
            std::cout << field.name << "\n";
        }
        else std::cout << "value NOT FOUND" << "\n"; // enter this line

        // Find: classof<Test>() == test->meta();
        const Structure& fields = classof<Test>()->fields();
        for (auto& var : fields) {
            printf("%s\n", var.name);
        }
        // print private variable deque
    }

    /* Srializer */ {
        RC<Test> test;
        Field    field = classof("Test")->field("deque");

        // UNSAFE
        uint8_t* ptr = test.as<uint8_t>();

        Deque<string>* deque = reinterpret_cast<Deque<string>*>(ptr += field.offset);
        
        // use private variable
        deque->push("[{SERIALIZE TEST}]"); // print { ["\[\{SERIALIZE TEST\}\]"] }
        // {} [] -> \[ \] \{ \}, for safe formatting

        // serialize
        std::cout << test->serialize();
    }
}
}