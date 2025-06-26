#ifndef LWE_CORE_TAG
#define LWE_CORE_TAG

#include "macro.hpp"

LWE_BEGIN
namespace core {

class Static {
    Static() noexcept                         = default;
    Static(const Static&) noexcept            = default;
    Static(Static&&) noexcept                 = default;
    Static& operator=(const Static&) noexcept = default;
    Static& operator=(Static&&) noexcept      = default;
};

enum class Uninit {
    UNINIT
};
static constexpr Uninit UNINIT = Uninit::UNINIT; // using

} // namespace core
LWE_END
#endif
