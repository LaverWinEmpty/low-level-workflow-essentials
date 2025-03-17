#ifndef LWE_CONTAINER_HEADER
#define LWE_CONTAINER_HEADER

#include "hal.hpp"

LWE_BEGIN

namespace stl {

namespace config {
static constexpr size_t DEF_SVO = 8; //!< default small vector optimization size
} // namespace config
using namespace config;

struct Container {
    virtual ~Container() noexcept {}
    virtual string serialize() const          = 0;
    virtual void   deserialize(const string&) = 0;

protected:
    /**
     * @brief serialize default
     */
    template<typename Derived> static string serialize(const Derived*);
public:
    /**
     * @brief deserialize default
     * @note  NEED push(const T&) / push (T&&)
     */
    template<typename Derived> static Derived deserialize(const string&);
};

} // namespace stl

LWE_END

#endif
