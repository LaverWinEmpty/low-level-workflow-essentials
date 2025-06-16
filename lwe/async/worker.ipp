#ifdef LWE_SYNC_WORKER
LWE_BEGIN
namespace async {

Worker::Worker(size_t count) : end(false), stop(false) {
    if (count < 1) {
        count = 1;
    }
    workers.reserve(count);
    for (int i = 0; i < count; ++i) {
        workers.emplace_back([this]() { run(); }); // parallel
    }
}

Worker::~Worker() {
    terminate();
}

void Worker::submit(Task in) {
    LOCKGUARD(lock) {
        if (!stop.load(std::memory_order_acquire)) {
            tasks.push(std::move(in));
        }
    }
    event.notify_one(); // wake up
}

void Worker::run() {
    while (!end.load(std::memory_order_relaxed)) {
        std::unique_lock guard(lock);
        event.wait(guard, [this]() {
                return end.load(std::memory_order_relaxed)  || // terminated
                       stop.load(std::memory_order_acquire) || // no more submit
                       !tasks.empty();                         // has message
            }
        );

        // check end and empty
        if (end.load(std::memory_order_relaxed)) {
            return; // return
        }

        // run
        Task task;
        if(tasks.shift(task)) {
            task();
        }

        // last task
        // use relaxed reason
        // - terminate() -> "stop = true" and "end = true"
        // - submit() -> "stop == true", not work
        // - "stop = ture" after "tasks.push()" -> use lock guard, safe
        // - "end = true" after "while(!end)" -> call event.wait(end), safe
        if (stop.load(std::memory_order_acquire) && tasks.empty()) {
            end.store(true, std::memory_order_relaxed); // set end flag, all thread joinable
        }
    }
}

void Worker::terminate() {
    stop.store(true, std::memory_order_release); // no more submissions accepted.
    std::call_once(flag,
        [this]() {
            event.notify_all(); // wake up all
            // join and wait
            for (auto& worker : workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }
    );
}

} // namespace async
LWE_END
#endif