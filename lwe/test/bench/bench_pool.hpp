#include "internal/bench.hpp"

#include "../../mem/allocator.hpp"
#include <boost/pool/singleton_pool.hpp>

static constexpr int COUNT = 1'000'000;

void* ptr[COUNT];

struct Type {
    Type() = default;
    ~Type() = default;

    char array[128];
};

int main() {
    Bench::introduce();

    std::cout << "NOTE: RELATIVELY SLOWER, AS IT SUPPORTS RELEASING UNUSED MEMORY\n";

    float defa, deff;
    /* NEW & DELETE */ {
        Bench a, f;

        /* STD */
        for(int i = 0; i < Bench::TRY; ++i) {
            a.once([&]() {
                for(int i = 0; i < COUNT; ++i) ptr[i] = new Type;
            }); // alloc
            f.once([&]() {
                for(int i = 0; i < COUNT; ++i) delete ptr[i];
            }); // free
        }

        a.output("DEFAULT NEW");
        f.output("DEFAULT FREE");

        defa = a.average();
        deff = f.average();
    }
    Bench::line();
    std::cout << std::endl;

    /* MEMORY POOL */ {
        Bench a, f, lwea, lwef;

        boost::pool<> bst{ 128 };
        lwe::mem::Pool lwep{ 128 };

        // warm-up
        for(int i = 0; i < COUNT; ++i) ptr[i] = bst.malloc();
        for(int i = 0; i < COUNT; ++i) bst.free(ptr[i]);

        /* BOOST */
        for(int i = 0; i < Bench::TRY; ++i) {
            a.once([&]() { for(int i = 0; i < COUNT; ++i) ptr[i] = bst.malloc(); }); // alloc
            f.once([&]() { for(int i = 0; i < COUNT; ++i) bst.free(ptr[i]); });      // free
        }

        // warm-up
        for(int i = 0; i < COUNT; ++i) ptr[i] = lwep.allocate<void>();
        for(int i = 0; i < COUNT; ++i) lwep.deallocate<void>(ptr[i]);

        /* LWE */
        for(int i = 0; i < Bench::TRY; ++i) {
            lwea.once([&]() { for(int i = 0; i < COUNT; ++i) ptr[i] = lwep.allocate<void>(); }); // alloc
            lwef.once([&]() { for(int i = 0; i < COUNT; ++i) lwep.deallocate<void>(ptr[i]); });  // free
        }

        a.output("BOOST ALLOCATE");
        lwea.output("LWE ALLOCATE");
        lwea.from(a.average());

        f.output("BOOST DEALLOCATE");
        lwef.output("LWE DEALLOCATE");
        lwef.from(f.average());
    }

    /* MEMORY POOL (RESERVED) */ {
        Bench a, f, lwea, lwef;

        boost::pool<>  bst{ 128, COUNT };
        lwe::mem::Pool lwep{ 128, 1, COUNT };

        // warm-up
        for(int i = 0; i < COUNT; ++i) ptr[i] = bst.malloc();
        for(int i = 0; i < COUNT; ++i) bst.free(ptr[i]);

        /* BOOST */
        for(int i = 0; i < Bench::TRY; ++i) {
            a.once([&]() { for(int i = 0; i < COUNT; ++i) ptr[i] = bst.malloc(); }); // alloc
            f.once([&]() { for(int i = 0; i < COUNT; ++i) bst.free(ptr[i]); });      // free
        }

        // warm-up
        for(int i = 0; i < COUNT; ++i) ptr[i] = lwep.allocate<void>();
        for(int i = 0; i < COUNT; ++i) lwep.deallocate<void>(ptr[i]);

        /* LWE */
        for(int i = 0; i < Bench::TRY; ++i) {
            lwea.once([&]() { for(int i = 0; i < COUNT; ++i) ptr[i] = lwep.allocate<void>(); }); // alloc
            lwef.once([&]() { for(int i = 0; i < COUNT; ++i) lwep.deallocate<void>(ptr[i]); });  // free
        }

        a.output("BOOST ALLOCATE (RESERVED)");
        lwea.output("LWE ALLOCATE (RESERVED)");
        lwea.from(a.average());

        f.output("BOOST DEALLOCATE (RESERVED)");
        lwef.output("LWE DEALLOCATE (RESERVED)");
        lwef.from(f.average());
    }

    /* THREAD SAFETY MEMORY POOL */ {
        Bench a, f, lwea, lwef;

        // warm-up
        for(int i = 0; i < COUNT; ++i) ptr[i] = boost::singleton_pool<void, 128>::malloc();
        for(int i = 0; i < COUNT; ++i) boost::singleton_pool<void, 128>::free(ptr[i]);

        /* BOOST */
        for (int i = 0; i < Bench::TRY; ++i) {
            // alloc
            a.once([]() {
                for(int i = 0; i < COUNT; ++i) {
                    ptr[i] = boost::singleton_pool<void, 128>::malloc();
                    new(ptr[i]) Type();
                }
            });

            // free
            f.once([]() {
                for(int i = 0; i < COUNT; ++i) {
                    ((Type*)ptr[i])->~Type();
                    boost::singleton_pool<void, 128>::free(ptr[i]);
                }
            });
        }

        static constexpr size_t BLOCKS = COUNT / (4096 / sizeof(Type) / 2);
        lwe::mem::Allocator<Type>::generate(BLOCKS);

        // warm-up
        for(int i = 0; i < COUNT; ++i) ptr[i] = lwe::mem::Allocator<Type>::allocate();
        for(int i = 0; i < COUNT; ++i) lwe::mem::Allocator<Type>::deallocate((Type*)ptr[i]);

        /* LWE */
        for(int i = 0; i < Bench::TRY; ++i) {
            // alloc
            lwea.once([]() {
                for(int i = 0; i < COUNT; ++i) ptr[i] = lwe::mem::Allocator<Type>::allocate();});

            // free
            lwef.once([]() {
                for(int i = 0; i < COUNT; ++i) lwe::mem::Allocator<Type>::deallocate((Type*)ptr[i]);
            });
        }

        a.output("THREAD SAFETY BOOST ALLOCATE");
        lwea.output("THREAD SAFETY LWE ALLOCATE");
        lwea.from(a.average());

        f.output("THREAD SAFETY BOOST DEALLOCATE");
        lwef.output("THREAD SAFETY LWE DEALLOCATE");
        lwef.from(f.average());

        std::cout << "\n";
        std::cout << "LWE THREAD SAFETY ALLOCATE PERFORMANCE COMPARED TO `new`\n";
        lwea.from(defa);
        std::cout << "LWE THREAD SAFETY ALLOCATE PERFORMANCE COMPARED TO `delete`\n";
        lwef.from(deff);
    }
}
