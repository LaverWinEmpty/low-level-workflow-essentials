#ifndef LWE_CORE_TAG
#define LWE_CORE_TAG

#include "macro.hpp"

LWE_BEGIN
namespace core {

class Noncopy {
private:
    Noncopy(const Noncopy&)            = delete;
    Noncopy& operator=(const Noncopy&) = delete;
    
public:
    Noncopy() = default;
    ~Noncopy() = default;
};

class Nomove {
private:
    Nomove(Nomove&&)            = delete;
    Nomove& operator=(Nomove&&) = delete;

public:
    Nomove() = default;
    ~Nomove() = default;
};

enum class Uninitialized {
    UNINIT
};
using enum Uninitialized;

}
LWE_END
#endif