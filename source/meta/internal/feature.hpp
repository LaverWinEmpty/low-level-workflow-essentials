#ifndef LWE_META_FEATURE
#define LWE_META_FEATURE

#include "registry.hpp"
#include "reflector.hpp"

LWE_BEGIN
namespace meta {

//! @brief type codes
enum class Keyword : uint8_t {
    UNREGISTERED,
    VOID,
    SIGNED_INT,
    SIGNED_CHAR,
    SIGNED_SHORT,
    SIGNED_LONG,
    SIGNED_LONG_LONG,
    UNSIGNED_SHORT,
    UNSIGNED_INT,
    UNSIGNED_CHAR,
    UNSIGNED_LONG,
    UNSIGNED_LONG_LONG,
    BOOL,
    CHAR,
    WCHAR_T,
    FLOAT,
    DOUBLE,
    LONG_DOUBLE,
    ENUM,
    CLASS,
    UNION,
    POINTER,
    REFERENCE,
    FUNCTION,
    STD_STRING,
    STL_DEQUE,
    CONST,
};

//! my custom container interface
struct Container {
    virtual ~Container() noexcept;
    virtual string serialize() const          = 0;
    virtual void   deserialize(const string&) = 0;
};

//! @brief get container type code structur
template<typename, typename = std::void_t<>> struct ContainerCode {
    static constexpr Keyword VALUE = Keyword::UNREGISTERED;
};

//! @brief unused type
enum class Registered : bool {
    REGISTERED = 1
};

template<typename T> constexpr Keyword typecode(); //!< get type code
constexpr const char*                  typestring(Keyword);     //!< reflect type name string by enum

template<typename T> constexpr bool isSTL();                    //!< check container explicit
template<typename T> constexpr bool isSTL(const T&);            //!< check container implicit
template<> bool                     isSTL<Keyword>(const Keyword&); //!< check container type code

//! @brief pre-registered metadata of T, return value is unused
template<typename T> Registered registclass();

//! @brief pre-registered metadata of T, return value is unused
template<typename T> Registered registenum();

//! @brief pre-registered methods of T, return value is unused
template<typename T> Registered registmethod();


}
LWE_END
#include "feature.ipp"
#endif