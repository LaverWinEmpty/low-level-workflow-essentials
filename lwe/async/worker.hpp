#ifndef LWE_SYNC_WORKER
#define LWE_SYNC_WORKER

#include <future>

#include "../stl/deque.hpp"

LWE_BEGIN
namespace async {

class Worker {
    using Task = std::function<void()>;

public:
    //! @param [in] size_t thread pool count
    Worker(size_t = 1);

public:
    //! @brief join (shutdown)
    ~Worker();

public:
    //! @brief insert lambda
    void submit(Task);

private:
    //! worker thread work
    void run();

public:
    //! join
    void terminate();

private:
    std::vector<std::thread> workers; //!< thread pool
    std::condition_variable  event;   //!< submit wake condition
    std::mutex               lock;    //!< cv mutex
    std::mutex               mtx;     //!< container mutex
    stl::Deque<Task>         tasks;   //!< task queue
    std::once_flag           flag;    //!< terminate called flag
    std::atomic_bool         stop;    //!< submit stop flag
    std::atomic_bool         end;     //!< worker end flag
}; // namespace async

}
LWE_END
#include "worker.ipp"
#endif