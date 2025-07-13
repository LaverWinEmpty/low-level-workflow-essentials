#ifndef LWE_UTIL_TIMER
#define LWE_UTIL_TIMER

#include "../base/base.h"
#include "../mem/block.hpp"

LWE_BEGIN
namespace util {

class Timer {
    using Clock = std::chrono::steady_clock;
    using MS    = std::chrono::milliseconds;

    static constexpr float       DEFAULT_MAX    = 3599999.99f; // 999:59:59.99
    static constexpr const char* DEFAULT_FORMAT = "%Y-%m-%d %H:%M:%S";

private:
    //! @note "0000-00-00T00:00:00" -> 20 (without "Z" or "UTC+00:00")
    using StringProxy = mem::Block<20>;

public:
    Timer() = default;

public:
    //! @brief timer reset
    void reset();

public:
    //! @brief get timer elapsed sec
    float sec() const;

public:
    //! @brief get time, max: 999:59:59.99
    //! @param [in] true: "0:0:0.99" / false: "0:0:0"
    StringProxy timestamp(bool = true) const;

public:
    //! @brief get process timer
    static const Timer& process();

public:
    //! @brief get system timestamp
    //! @param [in] true: UTC
    //! @note the delimiter can only be 1 byte
    static StringProxy system(const StringView = DEFAULT_FORMAT, bool = false);

public:
    //! @brief get system timestamp (default format)
    //! @param [in] true: UTC
    static StringProxy system(bool);

private:
    Clock::time_point last = Clock::now();

private:
    static Timer statics; //!< process timer
};

Timer Timer::statics;

// delta timer
class Tick: Static {
    static constexpr float NOSIE_FILTER = 0.00005f; //!< sec, ignore micro delays
    static constexpr int   FRAME_SKIP   = 10;       //!< fps 60 -> 0.16 sec (delta time clamping)

    // container for current and last values (float or int)
    struct Accumulator {
        // used instead of template for simplicity and locality.
        union {
            float f;
            int   n = 0;
        } curr, last;
    };

public:
    //! @brief initialize
    //! @param [in] float for tick time step
    static void initialize(float = 60);

public:
    //! @brief update
    static void update();

public:
    //! @brief get delta time 0.1 ms ~ 0.1 sec
    //! @param [in] bool false == ignore time scale, default: true
    static float dynamic(bool = true);

public:
    //! @brief get delta time unit 0.016 sec (based on 60fps target)
    //! @param [in] bool false == ignore time scale, default: true
    static float fixed(bool = true);

public:
    //! @brief get last frame count
    static int fps();

public:
    //! @brief get tick time to count
    //! @note  Tick::count() * Tick::rate() = Tick::fixed()
    static int count();

public:
    //! @brief get tick time step
    //! @note  Tick::rate() * Tick::count() = Tick::fixed()
    static float step();

public:
    static void  timescale(float); //!< set timescale
    static float timescale();      //!< get timescale

public:
    static void  refresh(float); //!< set fps refresh sec
    static float refresh();      //!< get fps refresh sec

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

} // namespace util
LWE_END
#include "timer.ipp"
#endif
