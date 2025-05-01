#ifndef LWE_CONTAINER
#define LWE_CONTAINER

#include "../meta/meta.hpp"

LWE_BEGIN
namespace meta {

struct Container {
    virtual ~Container() noexcept {}
    virtual string stringify() const    = 0;
    virtual void   parse(const string&) = 0;

protected:
    /**
     * @brief stringify default
     */
    template<typename Derived> static string stringify(const Derived*);
public:
    /**
     * @brief parse default
     * @note  NEED push(const T&) / push (T&&)
     */
    template<typename Derived> static Derived parse(const string&);
};

template<typename T> constexpr bool isSTL();                    //!< check container explicit
template<typename T> constexpr bool isSTL(const T&);            //!< check container implicit
template<> bool                     isSTL<EType>(const EType&); //!< check container type code


// get container type code structur
template<typename, typename = std::void_t<>> struct ContainerCode {
    static constexpr EType VALUE = EType::UNREGISTERED;
};

} // namespace meta
LWE_END
#endif
