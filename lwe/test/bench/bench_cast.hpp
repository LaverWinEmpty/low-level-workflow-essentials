#include "internal/bench.hpp"

#include "../../meta/meta.h"

#define NAME(X) Inheritance##X

#define DECLARE(PREVIOUS, CURRENT)                               \
    struct NAME(CURRENT): NAME(PREVIOUS) {                       \
        CLASS_BODY(Inheritance##CURRENT, Inheritance##PREVIOUS); \
    };                                                           \
    REGISTER_FIELD_BEGIN(Inheritance##CURRENT);                  \
    REGISTER_FIELD_END;

#define DECLARE_(VALUE) DECLARE(PREV(VALUE - 1), VALUE)

struct Inheritance0 : LWE::Object {
    CLASS_BODY(Inheritance0, LWE::Object);
};
REGISTER_FIELD_BEGIN(Inheritance0);
REGISTER_FIELD_END;

DECLARE(0,  1);
DECLARE(1,  2);
DECLARE(2,  3);
DECLARE(3,  4);
DECLARE(4,  5);
DECLARE(5,  6);
DECLARE(6,  7);
DECLARE(7,  8);
DECLARE(8,  9);
DECLARE(9,  10);
DECLARE(10, 11);
DECLARE(11, 12);
DECLARE(12, 13);
DECLARE(13, 14);
DECLARE(14, 15);
DECLARE(15, 16);
DECLARE(16, 17);
DECLARE(17, 18);
DECLARE(18, 19);
DECLARE(19, 20);
DECLARE(20, 21);
DECLARE(21, 22);
DECLARE(22, 23);
DECLARE(23, 24);
DECLARE(24, 25);
DECLARE(25, 26);
DECLARE(26, 27);
DECLARE(27, 28);
DECLARE(28, 29);
DECLARE(29, 30);
DECLARE(30, 31);

int main() {
    Bench b;
    b.introduce();

    using LWE::Object;

    static constexpr int LOOP = 1'000'000;
    
    Object* test = new Inheritance31;

    volatile bool    flag;
    volatile Object* ptr;

    b.loop([&]() { 
        for (int i = 0; i < LOOP; ++i) {
            ptr = dynamic_cast<Inheritance0*>(test);
            ptr = dynamic_cast<Inheritance1*>(test);
            ptr = dynamic_cast<Inheritance2*>(test);
            ptr = dynamic_cast<Inheritance3*>(test);
            ptr = dynamic_cast<Inheritance4*>(test);
            ptr = dynamic_cast<Inheritance5*>(test);
            ptr = dynamic_cast<Inheritance6*>(test);
            ptr = dynamic_cast<Inheritance7*>(test);
            ptr = dynamic_cast<Inheritance8*>(test);
            ptr = dynamic_cast<Inheritance9*>(test);
            ptr = dynamic_cast<Inheritance10*>(test);
            ptr = dynamic_cast<Inheritance11*>(test);
            ptr = dynamic_cast<Inheritance12*>(test);
            ptr = dynamic_cast<Inheritance13*>(test);
            ptr = dynamic_cast<Inheritance14*>(test);
            ptr = dynamic_cast<Inheritance15*>(test);
            ptr = dynamic_cast<Inheritance16*>(test);
            ptr = dynamic_cast<Inheritance17*>(test);
            ptr = dynamic_cast<Inheritance18*>(test);
            ptr = dynamic_cast<Inheritance19*>(test);
            ptr = dynamic_cast<Inheritance20*>(test);
            ptr = dynamic_cast<Inheritance21*>(test);
            ptr = dynamic_cast<Inheritance22*>(test);
            ptr = dynamic_cast<Inheritance23*>(test);
            ptr = dynamic_cast<Inheritance24*>(test);
            ptr = dynamic_cast<Inheritance25*>(test);
            ptr = dynamic_cast<Inheritance26*>(test);
            ptr = dynamic_cast<Inheritance27*>(test);
            ptr = dynamic_cast<Inheritance28*>(test);
            ptr = dynamic_cast<Inheritance29*>(test);
            ptr = dynamic_cast<Inheritance30*>(test);
            ptr = dynamic_cast<Inheritance31*>(test);
        }
    });
    b.output("DYNAMIC CAST RESULT");
    float temp = b.average();

    b.loop([&]() {
        for(int i = 0; i < LOOP; ++i) {
            flag = test->isof<Inheritance0*>();
            flag = test->isof<Inheritance1*>();
            flag = test->isof<Inheritance2*>();
            flag = test->isof<Inheritance3*>();
            flag = test->isof<Inheritance4*>();
            flag = test->isof<Inheritance5*>();
            flag = test->isof<Inheritance6*>();
            flag = test->isof<Inheritance7*>();
            flag = test->isof<Inheritance8*>();
            flag = test->isof<Inheritance9*>();
            flag = test->isof<Inheritance10*>();
            flag = test->isof<Inheritance11*>();
            flag = test->isof<Inheritance12*>();
            flag = test->isof<Inheritance13*>();
            flag = test->isof<Inheritance14*>();
            flag = test->isof<Inheritance15*>();
            flag = test->isof<Inheritance16*>();
            flag = test->isof<Inheritance17*>();
            flag = test->isof<Inheritance18*>();
            flag = test->isof<Inheritance19*>();
            flag = test->isof<Inheritance20*>();
            flag = test->isof<Inheritance21*>();
            flag = test->isof<Inheritance22*>();
            flag = test->isof<Inheritance23*>();
            flag = test->isof<Inheritance24*>();
            flag = test->isof<Inheritance25*>();
            flag = test->isof<Inheritance26*>();
            flag = test->isof<Inheritance27*>();
            flag = test->isof<Inheritance28*>();
            flag = test->isof<Inheritance29*>();
            flag = test->isof<Inheritance30*>();
            flag = test->isof<Inheritance31*>();
        }
    });
    b.output("Object::isof RESULT");
    b.from(temp);

    return 0;
}
