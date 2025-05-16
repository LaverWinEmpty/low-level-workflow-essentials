#include "macro.hpp"

LWE_BEGIN

class Noncopy {
protected:
    Noncopy(const Noncopy&)            = delete;
    Noncopy& operator=(const Noncopy&) = delete;

public:
    Noncopy() = default;
    ~Noncopy() = default;
};

class Nomove {
protected:
    Nomove(Nomove&&)            = delete;
    Nomove& operator=(Nomove&&) = delete;
};

enum class Uninitialized {
    UNINIT
};
using enum Uninitialized;

LWE_END