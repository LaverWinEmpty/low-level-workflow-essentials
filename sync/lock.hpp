#ifndef LWE_THREAD_LOCK
#define LWE_THREAD_LOCK

#include "../common/common.hpp"

LWE_BEGIN
namespace sync {

/**
 * @brief spin lock
 */
class Lock {
public:
    using Function = std::function<int(int)>;

private:
    static constexpr std::thread::id NOBODY        = std::thread::id();
    static constexpr int             BEGIN_ATTEMP  = 1;
    static constexpr int             BEGIN_BACKOFF = 1;

public:
    //! @note default backoff loop 4095 / max sleep 4.095 ms
    //! @param [in] backoff  loop count and sleep microseconds
    //! @param [in] Function backoff calculator [in: current loop / out: next loop]
    Lock(int backoff = 12, Function = [](int x) { return x * 2; });

public:
    void lock();
    void unlock();

public:
    const int BACKOFF;  //!< backoff limit
    Function  delegate; //!< backoff calculator

public:
    std::atomic<std::thread::id> owner;  //!< lock owner
    int                          locked; //!< lock counter
};

} // namespace thread
LWE_END

#include "lock.ipp"
#endif