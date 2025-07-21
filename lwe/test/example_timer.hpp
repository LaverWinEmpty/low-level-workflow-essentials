#include "iostream"
#include "../util/timer.hpp"

namespace test {

lwe::util::Timer timer; // init

float delta = 0;
float tick  = 0;
float pulse = 0; // time check

void example_timer_loop() {
    pulse = timer.sec();

    lwe::util::Tick::update(); // Tick Require initialize and update

    // false == ignore time scale (defualt: true)
    delta += lwe::util::Tick::dynamic(false); // dynamic delta time
    tick  += lwe::util::Tick::fixed(false);   // fixed delta time

    // initialize fps == 60 / timestep 0.016
    // udapte gap: 0.234 sec
    // e.g. dynamic(): 0.16  sec (limit 0.16  sec)
    // e.g. fixed():   0.224 sec (unit  0.016 sec)

    if(pulse >= 1) {
        // get system timestamp
        // param default: const char* = "%Y-%m-%d %H:%M:%S", bool utc = false
        std::cout << "System: " << lwe::util::Timer::system() << "\n";

        // get process timestamp
        // param true == 24h -> 1d 00 (default: false)
        std::cout << "Global: " << lwe::util::Timer::process().timestamp() << "\n";

        // get timestamp
        // param false == hide millisec (default: true)
        std::cout << "Timer:  " << timer.timestamp() << "\n";
        // or called sec() to get float seconds

        std::cout << "Tick:   " << tick << " sec\n";
        std::cout << "Delta:  " << delta << " sec\n";

        // fixed() tick detail
        std::cout << "Fixed Tick detail: " << " [FPS] " << lwe::util::Tick::fps() << // get fps
            " [Tick] " << lwe::util::Tick::count() <<                                // get tick count
            " [Step] " << lwe::util::Tick::step() <<                                 // get frame step
            "\n\n";

        // Dispersible: fixed == count() * step()

        timer.reset();
    }
}

void example_timer() {
    // default: 60 fps (timestep == 0.016)
    lwe::util::Tick::initialize(100);

    // set fps refrash sec
    lwe::util::Tick::refresh(0.5);

    // set time scale (default 1)
    lwe::util::Tick::timescale(2);

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
