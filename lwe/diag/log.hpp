#ifndef LWE_DIAG_LOG
#define LWE_DIAG_LOG

#include "../base/base.h"
#include "../util/timer.hpp"
#include "../async/worker.hpp"
#include "../mem/ptr.hpp"
#include "diag.h"

LWE_BEGIN
namespace diag {

//! @brief log level
enum class Verbosity : uint8_t {
    INFO,
    WARNING,
    ERROR
};

constexpr enum Verbosity INFO    = Verbosity::INFO;
constexpr enum Verbosity WARNING = Verbosity::WARNING;
constexpr enum Verbosity ERROR   = Verbosity::ERROR;

//! @brief log handler
class Logger {
    friend class Log;

public:
    Logger() = default;

protected:
    virtual void onWrite(const string& in, Verbosity type) {
        std::cerr << in << std::endl; // default
    }

protected:
    virtual bool onCheck(Verbosity in) {
        return in >= level; // default
    }

public:
    void write(const Alert& in, Verbosity type = INFO) {
        static thread_local char buffer[256];
        if(onCheck(type) == false) {
            return;
        }
        snprintf(buffer, sizeof(buffer),
            "[%s](%s): %s",
            static_cast<const char*>(util::Timer::system()),
            static_cast<const char*>(util::Timer::process()),
            in.what()
        );
        onWrite(buffer, type);
    }

private:
    Verbosity level = Verbosity::INFO;
};


class Log {
public:
    template<typename T> static void add(Verbosity in = INFO) {
        worker.submit(
            [in]() {
                Logger* handle = new T(); // no catch
                handle->level = in;

                // worker is single thread, no loack
                instance.handles.push_back(handle);
            }
        );
    }

public:
    static void write(const Alert& in, Verbosity type = INFO) {
        worker.submit(
            [in, type]() {
                for(auto& itr : instance.handles) {
                    itr->write(in, type);
                }
            }
        );
    }

public:
    template<typename T> static void write(const Expected<T>& in) {
        // succeeded
        if(in) {
            write(error(SUCCESS), INFO);
        }
        else write(in, ERROR);
    }

private:
    Log() = default;

public:
    ~Log() {
        worker.terminate(); // wait
        for(auto& itr : handles) {
            delete itr;
        }
    }

private:
    inline static lwe::async::Worker worker;
    std::vector<Logger*> handles; 

private:
    static Log instance;
};

Log Log::instance;


}
LWE_END
// #include "log.ipp"
#endif