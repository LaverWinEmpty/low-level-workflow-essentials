#include "iostream"
#include "../util/timer.hpp"

namespace test {

using namespace lwe::util;

Timer timer; // init

float delta = 0;
float tick  = 0;
float pulse = 0; // time check

void example_timer_loop() {
    pulse = timer.sec();

    Tick::update(); // Tick Require initialize and update

    // false == ignore time scale (defualt: true)
    delta += Tick::dynamic(false); // dynamic delta time
    tick  += Tick::fixed(false);   // fixed delta time

    // initialize fps == 60 / timestep 0.016
    // udapte gap: 0.234 sec
    // e.g. dynamic(): 0.16  sec (limit 0.16  sec)
    // e.g. fixed():   0.224 sec (unit  0.016 sec)

    if(pulse >= 1) {
        // get system timestamp
        // param default: const char* = "%Y-%m-%d %H:%M:%S", bool utc = false
        std::cout << "System: " << Timer::system() << "\n";

        // get process timestamp
        // param true == 24h -> 1d 00 (default: false)
        std::cout << "Global: " << Timer::process().timestamp() << "\n";

        // get timestamp
        // param false == hide millisec (default: true)
        std::cout << "Timer:  " << timer.timestamp() << "\n";
        // or called sec() to get float seconds

        std::cout << "Tick:   " << tick << " sec\n";
        std::cout << "Delta:  " << delta << " sec\n";

        // fixed() tick detail
        std::cout << "Fixed Tick detail: " << " [FPS] " << Tick::fps() << // get fps
            " [Tick] " << Tick::count() <<                                // get tick count
            " [Step] " << Tick::step() <<                                 // get frame step
            "\n\n";

        // Dispersible: fixed == count() * step()

        timer.reset();
    }
}

void example_timer() {
    /* === QUICK START === */
    const Timer& timer = Timer::process();              // program timer
    const_cast<Timer&>(timer).reset();                  // reset (non-const)
    float       sec = timer.sec();                      // get second
    std::string ts  = std::string(timer.timestamp());   // get timestamp (char*), param: show ms (default: true)
    std::cout << std::string(timer.system()) << "\n\n"; // get system time (char*), param: utc (default: false)

    /* === DETAIL TEST === */

    // default: 60 fps (timestep == 0.016 == 1 tick)
    Tick::initialize(100);

    // set fps refrash sec
    Tick::refresh(0.5);

    // set time scale (default 1)
    Tick::timescale(2);

    for(int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Error induction.
        example_timer_loop();
    }

    for(int i = 0; i < 50; ++i) {
        // Dynamic time catches up with fixed time.
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        example_timer_loop();
    }
}

} // namespace test
