#include <chrono>

LWE_BEGIN
namespace util {

void Timer::reset() {
    last = Clock::now();
}

float Timer::sec() const {
    return static_cast<float>(std::chrono::duration_cast<MS>(Clock::now() - last).count() * 0.001);
}

auto Timer::timestamp(bool millisec) const -> StringProxy {
    StringProxy out;

    // get and limit
    float elapsed = sec();
    if(elapsed >= DEFAULT_MAX) {
        elapsed = DEFAULT_MAX;
    }

    // get sec
    int seconds = int(elapsed); // to int

    // get time
    int hour = seconds / 3'600;
    int min  = (seconds / 60) % 60;
    int sec  = seconds % 60;
    int ms   = 0; // optional

    // get ms
    if(millisec) {
        ms = int((elapsed - seconds) * 100.f);
    }

    // branch
    if(millisec) {
        snprintf(out.data(), sizeof(StringProxy), "%03d:%02d:%02d.%02d", hour, min, sec, ms);
    }
    else snprintf(out.data(), sizeof(StringProxy), "%03d:%02d:%02d", hour, min, sec);

    return out;
}

const Timer& Timer::process() {
    return statics;
}

auto Timer::system(const StringView format, bool utc) -> StringProxy {
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
    StringProxy out;
    size_t last = std::strftime(out.data(), sizeof(StringProxy), format.data(), &info);
    return out;
}

auto Timer::system(bool utc) -> StringProxy { 
    return system(DEFAULT_FORMAT, utc);
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
