#include "../mem/ptr.hpp"

namespace test {

#pragma region CHORE
using namespace LWE;
struct Dummy_ptr {
    Dummy_ptr() = default;
    Dummy_ptr(int in): value(in) { }
    template<typename T> friend T& operator<<(T& os, const Dummy_ptr& in) {
        os << in.value;
        return os;
    }
    int value = 0;
};
using Ptr = LWE::mem::Ptr<Dummy_ptr>;

template<typename T>
void output_ptr(const mem::Ptr<T>& ptr) {
    std::cout << ptr.get() << " ... ";
    if(ptr) {
        std::cout << "(" << *ptr << ")";
    }
    else std::cout << "NULL";
    std::cout << std::endl; // address / reference count
}
#pragma endregion

void example_ptr_main() {
    /* default constructor */ {
        Ptr ptr;
        output_ptr(ptr);

        Ptr null = nullptr; // nullptr (call `Ptr(T*, Deleter)`)
        output_ptr(null);
    }

    /* parameter constructor*/ {
        Ptr a = Ptr(1);
        output_ptr(a);

        Dummy_ptr value = 2;
        Ptr       c     = value;
        output_ptr(c);

        Ptr b(3);
        output_ptr(b);

        Ptr d = new Dummy_ptr(4);
        output_ptr(d);
    }

    /* operator= */ {
        Ptr a = 100;
        Ptr b = a; // copy constructor
        *a    = 5; // weak
        output_ptr(b);

        Ptr c;
        c  = b; // operator=
        *a = 6; // weak
        output_ptr(c);

        a = 7;
        a = a; // self-assignment
        // a = a = 7; // UB
        output_ptr(a);
    }

    /* move, get */ {
        Ptr a;
        Ptr b = 8;
        a     = std::move(b);
        output_ptr(a); // output_ptr
        output_ptr(b); // output_ptr NULL

        *a = 9;
        printf("%p ... ", a.as<void>());  // casting
        printf("(%d)\n", a.get()->value); // get

        Dummy_ptr* get_1 = a.get();                    // get
        Dummy_ptr* get_2 = static_cast<Dummy_ptr*>(a); // get by cast
        void*      get_3 = a.as<void>();               // casting
    }

    /* about desturcor */ {
        // destructor set nullptr -> memory leak
        Dummy_ptr* heap = new Dummy_ptr;
        { Ptr c = Ptr(heap, nullptr); }
        delete heap; // safe

        // custom deallocator
        Ptr d = Ptr((Dummy_ptr*)malloc(sizeof(Dummy_ptr)), [](void* in) { free(in); });
    }

    /* dangling check */ {
        Ptr weak;

        /* valid case */ {
            Ptr owner = new Dummy_ptr;
            weak      = owner;

            *owner = 10;
            if(weak.valid()) {
                output_ptr(weak);
            }

            *owner = 11;
            if(weak.owned()) {
                output_ptr(weak); // not owner
            }

            weak.own(); // set owner
            *owner = 12;
            if(weak.owned()) {
                output_ptr(weak); // not owner
            }

            owner.own();
            // free owner
        }

        /* invalid case */ {
            // use memory pool, allow but not safe
            // safe: if(weak) *weak = value;
            *weak = 100;

            if(weak.valid()) {
                output_ptr(weak);
            }
            output_ptr(weak); // if (ptr) is false
        }

        /* clone */ {
            Ptr a;
            {
                Ptr b = new Dummy_ptr;
                a     = b; // weak

                b.clone(); // b is onwer, is failed
                a.clone(); // COW

                *a = 13;
                *b = 14;
                if(a.owned()) {
                    output_ptr(a);
                }
                if(b.owned()) {
                    output_ptr(b);
                }
            }
            *a = 15; // cloned, is safe
            output_ptr(a);
        }
    }
}

void example_ptr() {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    example_ptr_main();
}

} // namespace test
