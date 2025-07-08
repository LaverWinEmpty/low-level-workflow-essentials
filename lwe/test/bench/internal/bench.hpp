#ifndef LWE_BENCH
#define LWE_BENCH

#include "iostream"
#include "../../../util/timer.hpp"
#include <functional>

template<int TRY = 3> class Bench {
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
            avg /= TRY;
            idx  = 0; // init for next
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
        else printf("RES: %.3f SEC\n", avg); // only once
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
            result((1 - avg / sec) * 100);
        }
        line();
        std::cout << std::endl;
    }

    void to(float sec) const {
        line(true);
        if(check(sec, avg)) {
            result((avg / sec - 1) * 100);
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
            printf("RESULT: LATENCY INCREASED FROM ZERO\n");
            return false;
        }
        // exception
        else if(after == 0) {
            printf("RESULT: LATENCY REDUCED TO ZERO\n");
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

    float avg;
};

#endif
