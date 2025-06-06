#include <chrono>

LWE_BEGIN
namespace util {

class Timer {
    const float NOSIE_FILTER = 0.00005; // sec, ignore micro delays
    const int   FRAME_SKIP   = 10;      // fps 60 -> 0.16 sec (delta time clamping)

    using Clock = std::chrono::steady_clock;
    using MS    = std::chrono::milliseconds;
public:
    Timer() = default;

public:
    void reset() {
        last = Clock::now();
    }

public:
    //! @brief sec
    float lap() {
        return static_cast<float>(std::chrono::duration_cast<MS>(Clock::now() - last).count() * 0.001);
    }

//public:
//	string timestamp(const char* = nullptr);
//	string timestamp(string = "");

private:
    Clock::time_point last  = Clock::now();
};

// delta timer
class Tick : Static {
    // container for current and last values (float or int)
    struct Accumulator {
        // used instead of template for simplicity and locality.
        union {
            float f;
            int   n = 0;
        } curr, last;
    };

public:
    static void initialize(float fps) {
        // use on tick time
        tickstep = 1 / fps;
        
        // use on delta time
        skip = tickstep * FRAME_SKIP;

        // set 0
        tick.curr.f = 0;
        tick.last.f = 0;
        delta.curr.f = 0;
        delta.last.f = 0;
        frame.curr.n = 0;
        frame.last.n = 0;

        // reset timer
        timer.reset();
    }

public:
    static void update() {
        // frame count
        ++frame.curr.n;

        // get elapsed time (delta time)
        float dt = timer.lap();

        // accumulate
        // noise filtering
        if (dt > NOSIE_FILTER) {
            pulse        += dt;
            delta.curr.f += dt;
            tick.curr.f  += dt;
            timer.reset();
        }

        // calculate delta time clamping
        if (delta.curr.f >= skip) {
            delta.last.f =  skip;  // set limit
            delta.curr.f -= skip; // reduction
        }
        else {
            delta.last.f = delta.curr.f; // remainder
            delta.curr.f = 0;            // init
        }

        // calculate tick time
        tickcnt = 0; // init
        while (tick.curr.f >= tickstep) {
            tick.curr.f -= tickstep;
            ++tickcnt;
        }
        tick.last.f = tickcnt * tickstep;

        // calculate frame per seonds
        if (pulse >= interval) {
            pulse -= interval;
            frame.last.n = frame.curr.n; // set
            frame.curr.n = 0;            // init
        }
    }

public:
    //! @brief get delta time 0.1 ms ~ 0.1 sec
    //! @param [in] false == ignore time scale, default: true
    static float dynamic(bool scaled = true) {
        return scaled ? delta.last.f * scale : delta.last.f;
    }

public:
    //! @brief get delta time unit 0.016 sec (based on 60fps target)
    //! @param [in] false == ignore time scale, default: true
    static float fixed(bool scaled = true) {
        return scaled ? tick.last.f * scale : tick.last.f;
    }

public:
    //! @brief get last frame count
    static int fps() {
        return frame.last.n * correct;
    }

public:
    //! @brief get tick time to count
    //! @note  Tick::count() * Tick::rate() = Tick::fixed() 
    static int count() {
        return tickcnt;
    }

public:
    //! @brief get tick time step
    //! @note  Tick::rate() * Tick::count() = Tick::fixed() 
    static float step() {
        return tickstep;
    }

public:
    //! @brief get timescale
    static float timescale() {
        return scale;
    }
    //! @brief set timescale
    static void timescale(float in) {
        scale = in;
    }

public:
    //! @brief fps refresh sec
    static void refresh(float in) {
        pulse    = 0;
        interval = in;
        correct = 1 / interval;
    }
    //! @brief get fps refresh sec
    static float refresh() {
        return interval;
    }

public:
    //! @brief frame skip flag


private:
    inline static Timer timer; //!< main timer

    inline static Accumulator tick;    //!< fixed tick counter
    inline static Accumulator delta;   //!< delta time counter
    inline static Accumulator frame;   //!< frame counter
    inline static int         tickcnt; //!< last tick count

    inline static float tickstep = 0; //!< frame time step
    inline static float pulse    = 0; //!< for fps count
    inline static float interval = 1; //!< fps refresh
    inline static float correct  = 1; //!< frame count to per sec
    inline static float scale    = 1; //!< time scale use in delta / tick
    inline static float skip     = 0; //!< delta time frame skip
    
};

}
LWE_END