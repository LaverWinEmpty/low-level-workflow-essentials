#if (COMPILER == MSVC)
#    include <immintrin.h>
#endif

LWE_BEGIN
namespace async {

Lock::Lock(int sleep, int backoff, Function function):
    SLEEP(sleep),
    BACKOFF(backoff),
    delegate(function),
    owner(NOBODY),
    locked(0) {
    if(!function) {
        // default function
        delegate = [](int x) {
            return x * 2;
        };
    }
    else delegate = function;
}

void Lock::lock() {
    std::thread::id thisThread = std::this_thread::get_id();
    std::thread::id testThread = NOBODY;

    // reentrance == relaxed
    if(owner.load(std::memory_order_acquire) == thisThread) {
        ++locked; // not atmoic, but locked -> safe
        return;
    }

    int  backoff   = BEGIN_BACKOFF;
    int  attempt   = BEGIN_ATTEMPT;
    bool liberated = false;

    // try
    while(!owner.compare_exchange_weak(testThread, // from
                                       thisThread, // to
                                       std::memory_order_acquire)) {
        testThread = NOBODY; // reset

        // exchange failed, not onwer,
        liberated = false;

        // spin
        for(int i = 0; i < attempt; ++i) {
            // cpu pause instruction for better performance
#if (COMPILER == MSVC)
            _mm_pause();
#elif defined(__x86_64__) || defined(_M_X64)
            asm volatile("pause" ::: "memory");
#elif defined(__aarch64__)
            asm volatile("yield" ::: "memory");
#else
            volatile int dummy = 0;
            (void)dummy; // other os, avoid optimization
#endif
            // owner is free, try exchange immediately
            if(owner.load(std::memory_order_relaxed) == NOBODY) {
                liberated = true;
                break;
            }
        }

        // if true, break and try exchange
        if(liberated == false) {
            // sleep, limit default 2us (recommended)
            if(++backoff >= BACKOFF) {
                std::this_thread::sleep_for(std::chrono::microseconds(SLEEP));
                // initialize -> spin retry
                backoff = BEGIN_BACKOFF;
                attempt = BEGIN_ATTEMPT;
            }
            else attempt = delegate(attempt);
        }
    }

    // locked, enter only owner
    locked = 1;
}

void Lock::unlock() {
    std::thread::id current_thread = std::this_thread::get_id();

    // error
    // assert(owner.load(std::memory_order_relaxed) == current_thread);

    // TODO: Alert
    if (owner.load(std::memory_order_relaxed) != current_thread) {
        throw 0;
    }

    // free, count only owner
    if(--locked == 0) {
        owner.store(NOBODY, std::memory_order_release);
    }
}

} // namespace async
LWE_END
