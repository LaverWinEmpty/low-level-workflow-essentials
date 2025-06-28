#ifndef LWE_SYNC_LOCK
#define LWE_SYNC_LOCK

#include "../base/base.h"

LWE_BEGIN
namespace async {

//! @brief spin lock
class Lock {
public:
    using Function = std::function<int(int)>;

private:
    static constexpr std::thread::id NOBODY        = std::thread::id();
    static constexpr int             BEGIN_ATTEMPT = 1;
    static constexpr int             BEGIN_BACKOFF = 1;

public:
    //! @note default backoff loop 4095 / max sleep 4.095 ms
    //! @param [in] backoff  loop count and sleep microseconds
    //! @param [in] Function backoff calculator [in: current loop / out: next loop]
    //! @param [in] sleep    time to sleep when backoff limit is reached (microsecond)
    Lock(int sleep = 2, int backoff = 12, Function = nullptr);

public:
    void lock();
    void unlock();

public:
    const int SLEEP;    //!< sleep time
    const int BACKOFF;  //!< backoff limit
    Function  delegate; //!< backoff calculator

public:
    std::atomic<std::thread::id> owner;  //!< lock owner
    int                          locked; //!< lock counter
};

} // namespace async
LWE_END

#include "lock.ipp"
#endif
