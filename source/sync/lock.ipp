#ifdef LWE_SYNC_LOCK

LWE_BEGIN
namespace sync {

Lock::Lock(int backoff, Function function): BACKOFF(backoff), delegate(function), owner(NOBODY), locked(0) {}

void Lock::lock() {
    std::thread::id thisThread = std::this_thread::get_id();
    std::thread::id testThread = NOBODY;

    // reentrance == relaxed
    if(owner.load(std::memory_order_acquire) == thisThread) {
        ++locked;
        return;
    }

    int  backoff = BEGIN_BACKOFF;
    int  attemp  = BEGIN_ATTEMP;
    bool flag    = false;

    // try
    while(!owner.compare_exchange_weak(testThread, // from
                                       thisThread, // to
                                       std::memory_order_acquire)) {
        testThread = NOBODY; // reset

        for(int i = 0; i < attemp; ++i) {
            // go to try lock
            if(owner.load(std::memory_order_acquire) == NOBODY) {
                flag = true;
                break;
            }
        }

        if(flag == false) {
            // sleep
            std::this_thread::sleep_for(std::chrono::microseconds(attemp));

            // retry
            if(++backoff >= BACKOFF) {
                std::this_thread::yield();
                // initialize
                backoff = BEGIN_BACKOFF;
                attemp  = BEGIN_ATTEMP;
            } else attemp = delegate(attemp);
        } else flag = false;
    }

    locked = 1;
}

void Lock::unlock() {
    std::thread::id current_thread = std::this_thread::get_id();

    // error
    assert(owner.load(std::memory_order_relaxed) == current_thread);

    // free
    if(--locked == 0) {
        owner.store(NOBODY, std::memory_order_release);
    }
}

} // namespace thread
LWE_END
#endif