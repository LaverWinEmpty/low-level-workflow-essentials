#include "../mem/ptr.hpp"

#include "iostream"

namespace test {

void example_ptr() {
    using LWE::mem::Ptr;

    /* === QUICK START === */
    Ptr<int> ptr0 = {};          // nullptr
    Ptr<int> ptr1 = new int(42); // like `shared_ptr<int>(new int{42})`
    Ptr<int> ptr2 = int{ 42 };   // like `std::make_shared<int>(42)`

    // lambda: custom deleter, no capture
    Ptr<int> ptr3 = { (int*)malloc(4), [](int* in) {
                         free(in); // default: delete in;
                     } };

    ptr2 = ptr1; // weak copy

    ptr3 = nullptr; // release

    bool result = ptr2.own(); // transfer ownership

    if(!result) { } // ptr2 is nullptr or dangling
    else { }        // ptr2 is weak, or was the original owner.

    if(ptr2.owned()) { } // check ownership

    if(ptr2.valid()) { } // check dangling

    if(ptr2) { } // check dangling 2

    ptr1.clone(); // copy-on-write
    ptr2.clone(); // copy-on-write, but not work: it is owner

    int*  word = ptr1.get();      // get raw pointer
    char* byte = ptr2.as<char>(); // get raw pointer by cast

    int* ptr = static_cast<int*>(ptr3); // allow (explicit operator cast)

    /* === DETAIL  === */ {
        Ptr<int> weak;

        /* release test*/ {
            Ptr<int> owner = int{ 42 };

            weak = owner; // store
        }
        // owner is released

        // Ambiguity: dangling or null check
        if(!weak) {
            // But not really important
            std::cout << "weak is dangling or null\n";
        }
        else std::cout << "weak is OK\n";

        // SAFE CASE
        if(weak) {
            *weak = 100; // safe
            std::cout << *weak << "\n";
        }
        else {
            try {
                *weak = 100; // THROW EXCEPTION
            }
            catch(...) {
                std::cout << "weak is dangling or null\n";
            }
        }

        Ptr<int> dangling = weak; // WARNING: DANGLING COPY

        Ptr<int> owner2 = int{ 200 }; // address that are the same as the owner's block

        // WARNING CASE
        // get() is not throw exception
        std::cout << *dangling.get() << "\n"; // 200
        // Reason: use memory pool
        // And, `owner` has been confirmed that it has been released normally.
    }

    /* === EDGE CASE === */ {
        // NOTE: THREAD UNSAFE

        // release test
        Ptr<int> owner = int{};
        Ptr<int> weak  = owner;

        owner = nullptr; // release
        owner = nullptr; // release again, why not?
        weak  = nullptr; // why not?

        Ptr<int> ptr1 = int();
        // Ptr<int> ptr2 = ptr1.get(); // WARNING
        // ptr2          = nullptr;    // DOUBLE RELEASE

        // circulation test
        struct B;
        struct A {
            LWE::mem::Ptr<B> b;
        }; // A -> B
        struct B {
            LWE::mem::Ptr<A> a;
        }; // B -> A

        void *a, *b;
        {
            Ptr<A> aa = A{};
            Ptr<B> bb = B{};

            aa->b = bb;
            bb->a = aa;

            a = aa.get(); // get raw pointer
            b = bb.get(); // get raw pointer
        }
        Ptr<A> aa = A{}; // use memory pool -> same address
        Ptr<B> bb = B{}; // use memory pool -> same address

        std::cout << (a == aa.get()) << " " << (b == bb.get()) << "\n"; // 1 1, same address
        // THIS IS UNIQUE PTR, why not?

        // nullptr test
        Ptr<int> ptr = nullptr;
        // *ptr = 100; // WARNING: CRASH
    }
}

} // namespace test
