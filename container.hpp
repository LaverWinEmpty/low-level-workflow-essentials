#ifndef LWE_CONTAINER_HEADER
#define LWE_CONTAINER_HEADER

#include "meta.hpp"

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

template<typename T> constexpr bool isSTL();                    //!< check container explicit
template<typename T> constexpr bool isSTL(const T&);            //!< check container implicit
template<> bool                     isSTL<EType>(const EType&); //!< check container type code

// get container type code structur
template<typename, typename = std::void_t<>> struct ContainerCode {
    static constexpr EType VALUE = EType::UNREGISTERED;
};

#endif
