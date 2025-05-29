#ifndef LWE_META_CONTAINER
#define LWE_META_CONTAINER

#include "type.hpp"

LWE_BEGIN
namespace meta {

struct Container {
    virtual ~Container() noexcept {}
    virtual string serialize() const    = 0;
    virtual void   deserialize(const string&) = 0;

protected:
    /**
     * @brief stringify default
     */
    template<typename Derived> static string serialize(const Derived*);
public:
    /**
     * @brief parse default
     * @note  NEED push(const T&) / push (T&&)
     */
    template<typename Derived> static Derived deserialize(const string&);
};

// get container type code structur
template<typename, typename = std::void_t<>> struct ContainerCode {
    static constexpr EType VALUE = EType::UNREGISTERED;
};

} // namespace meta
LWE_END
#endif
