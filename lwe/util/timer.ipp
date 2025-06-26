#include <chrono>

LWE_BEGIN
namespace util {

struct Timer::String {
    //! @brief object time
    //! @param [in] compact: 86400.01 -> false "24:00:00" / true "01d_00:00:00"
    //! @param [in] detail:  86400.01 -> false "24:00:00" / true "24:00:00.01"
    String(const Timer* in, bool compact, bool detail) {
        std::ostringstream oss;

        // get and limit
        float elapsed = in->sec();
        if(elapsed >= MAX) {
            elapsed = MAX;
        }

        // get sec
        int seconds = int(elapsed); // to int

        // get time
        int hour = seconds / 3'600;
        int min  = (seconds / 60) % 60;
        int sec  = seconds % 60;
        int day  = 0; // optional
        int ms   = 0; // optional

        // get day (max: 41d)
        if(compact) {
            day   = seconds / 86'400; // max: 41
            hour %= 24;               // max: 15
        }

        // get ms
        if(detail) {
            ms = int((elapsed - seconds) * 100.f);
        }

        // branch
        if(compact) {
            if(detail) {
                snprintf(str, sizeof(str), "%02dd %02d:%02d:%02d.%02d", day, hour, min, sec, ms);
            }
            else snprintf(str, sizeof(str), "%02dd %02d:%02d:%02d", day, hour, min, sec);
        }
        else {
            if(detail) {
                snprintf(str, sizeof(str), "%03d:%02d:%02d.%02d", hour, min, sec, ms);
            }
            else snprintf(str, sizeof(str), "%03d:%02d:%02d", hour, min, sec);
        }
    }
    //! system time
    String(const char* format, bool utc) {
        std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm     info;
        if(utc) {
#if COMPILER == MSVC
            ::gmtime_s(&info, &t);
#else
            ::gmtime_r(&t, &info);
#endif
        }
        else {
#if COMPILER == MSVC
            localtime_s(&info, &t);
#else
            localtime_r(&t, &info);
#endif
        }
        std::strftime(str, 20, format, &info);
    }
    operator const char*() const { return str; }
private:
    char str[20] = { 0 }; //!< "1900-01-01_00:00:00"
};

void Timer::reset() {
    last = Clock::now();
}

float Timer::sec() const {
    return static_cast<float>(std::chrono::duration_cast<MS>(Clock::now() - last).count() * 0.001);
}

auto Timer::timestamp(bool ms) const -> String {
    return String(this, ms, false); // porxy
}

auto Timer::process(bool day) -> String {
    return String(&statics, day, false); // proxy
}

auto Timer::system(string format, bool utc) -> String {
    return String(format.c_str(), utc); // proxy
}

auto Timer::system(const char* format, bool utc) -> String {
    return String(format, utc); // proxy
}

void Tick::initialize(float fps) {
    // use on tick time
    tickstep = 1 / fps;

    // use on delta time
    skip = tickstep * FRAME_SKIP;

    // set 0
    tick.curr.f  = 0;
    tick.last.f  = 0;
    delta.curr.f = 0;
    delta.last.f = 0;
    frame.curr.n = 0;
    frame.last.n = 0;

    // reset timer
    timer.reset();
}

void Tick::update() {
    // frame count
    ++frame.curr.n;

    // get elapsed time (delta time)
    float dt = timer.sec();

    // accumulate
    // noise filtering
    if(dt > NOSIE_FILTER) {
        pulse        += dt;
        delta.curr.f += dt;
        tick.curr.f  += dt;
        timer.reset();
    }

    // calculate delta time clamping
    if(delta.curr.f >= skip) {
        delta.last.f  = skip; // set limit
        delta.curr.f -= skip; // reduction
    }
    else {
        delta.last.f = delta.curr.f; // remainder
        delta.curr.f = 0;            // init
    }

    // calculate tick time
    tickcnt = 0; // init
    while(tick.curr.f >= tickstep) {
        tick.curr.f -= tickstep;
        ++tickcnt;
    }
    tick.last.f = tickcnt * tickstep;

    // calculate frame per seonds
    if(pulse >= interval) {
        pulse        -= interval;
        frame.last.n  = frame.curr.n; // set
        frame.curr.n  = 0;            // init
    }
}

float Tick::dynamic(bool scaled) {
    return scaled ? delta.last.f * scale : delta.last.f;
}

float Tick::fixed(bool scaled) {
    return scaled ? tick.last.f * scale : tick.last.f;
}

int Tick::fps() {
    return int(frame.last.n * correct);
}

int Tick::count() {
    return tickcnt;
}

float Tick::step() {
    return tickstep;
}

void Tick::timescale(float in) {
    scale = in;
}

float Tick::timescale() {
    return scale;
}

void Tick::refresh(float in) {
    pulse    = 0;
    interval = in;
    correct  = 1 / interval;
}

float Tick::refresh() {
    return interval;
}

} // namespace util
LWE_END
