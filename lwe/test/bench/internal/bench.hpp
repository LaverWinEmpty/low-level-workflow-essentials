#ifndef LWE_BENCH
#define LWE_BENCH

#include "iostream"
#include "../../../util/timer.hpp"
#include <functional>

class Bench {
public:
    static constexpr int TRY = 5;

public:
    static void introduce() {
        std::string os = "Windows 11 "
#if _WIN64
                         "x64";
#else
                         "x32";
#endif

        std::string compiler = "MSVC 2022 v143 "
#ifdef NDEBUG
                               "Release (/O2)";
#else
                               "Debug";
#endif

        std::string language = "C++ "
#if (__cplusplus == 199711L)
                               "98/03";
#elif (__cplusplus == 201402L)
                               "14";
#elif (__cplusplus == 201703L)
                                "17";
#elif (__cplusplus >= 202002L)
                                "20 or later";
#endif

        std::cout <<
            "Version:  " << language << "\n"
            "Compiler: " << compiler << "\n"
            "OS:       " << os << "\n"
            "CPU:      Ryzen R7-3700x\n"
            "RAM:      DDR4-3200 / 8GB x 2\n";
        std::cout << std::endl;
    }

public:
    void loop(const std::function<void()>& fn) { while(once(fn)); }

    bool once(const std::function<void()>& fn) {
        if(idx == 0) {
            avg = 0; // init on first
        }

        timer.reset();
        fn(); // call
        arr[idx]  = timer.sec();
        avg      += arr[idx];

        ++idx;
        if(idx == TRY) {
            idx  = 0; // init for next
            avg /= TRY;

            rsd = 0;
            if constexpr(TRY > 1) {
                for(int i = 0; i < TRY; ++i) {
                    float temp  = (arr[i] - avg);
                    rsd        += temp * temp;
                }
                rsd = (std::sqrtf(rsd / TRY) / avg) * 100;
            }
            return false;
        }
        return true;
    }

    template<typename... Args> void output(Args&&... in) const {
        line();
        if constexpr(sizeof...(Args) != 0) {
            param(std::forward<Args>(in)...);
            std::cout << "\n";
            line(false);
        }
        if constexpr(TRY > 1) {
            for(int i = 0; i < TRY; ++i) {
                printf("%3d: %.3f SEC\n", i + 1, arr[i]);
            }
            line(false);
            printf("AVG: %.3f SEC\n", avg);
        }
        else {
            printf("RES: %.3f SEC\n", avg); // only once
        }
        printf("RSD: ¡¾%.3f%%\n", rsd); // standard deviation
    }

    static void line(bool thick = true) {
        if(thick) {
            std::cout << "================================================================================\n";
        }
        else std::cout << "--------------------------------------------------------------------------------\n";
    }

    float average() const { return avg; }

    void from(float sec) const {
        line(true);
        if(check(avg, sec)) {
            result((1 - sec / avg) * -100); // minus (faster) to plus
        }
        line();
        std::cout << std::endl;
    }

    void to(float sec) const {
        line(true);
        if(check(sec, avg)) {
            result((sec / avg - 1) * -100); // minus (faster) to plus
        }
        line();
        std::cout << std::endl;
    }

private:
    bool check(float before, float after) const {
        // same
        if(std::abs(before - after) < FLT_EPSILON) {
            printf("RESULT: NO DIFFERENCE\n");
            return false;
        }
        // exception
        else if(before == 0) {
            printf("RESULT: LATENCY REDUCED TO ZERO\n");
            return false;
        }
        // exception
        else if(after == 0) {
            printf("RESULT: LATENCY INCREASED FROM ZERO\n");
            return false;
        }
        return true;
    }

    void result(float percent) const {
        if(percent >= +0) {
            printf("RESULT: %.2f%% FASTER\n", percent);
        }
        else printf("RESULT: %.2f%% SLOWER\n", -percent);
    }

    template<typename Arg, typename... Args> void param(Arg&& in, Args&&... args) const {
        std::cout << std::forward<Arg>(in);
        param(std::forward<Args>(args)...);
    }

    template<typename Arg> void param(Arg&& in) const { std::cout << std::forward<Arg>(in); }

    LWE::util::Timer timer;

    float arr[TRY];

    int idx = 0;

    float avg = 0;
    float rsd = 0;
};

#endif
