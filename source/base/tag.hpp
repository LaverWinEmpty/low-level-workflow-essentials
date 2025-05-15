#include "macro.hpp"

LWE_BEGIN

class Noncopy {
private:
    Noncopy(const Noncopy&)            = delete;
    Noncopy& operator=(const Noncopy&) = delete;
};

class Nomove {
private:
    Nomove(Nomove&&)            = delete;
    Nomove& operator=(Nomove&&) = delete;
};

enum class Uninitialized {
    UNINIT
};
using enum Uninitialized;

LWE_END