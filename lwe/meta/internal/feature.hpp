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
    FLOAT,
    DOUBLE,
    LONG_DOUBLE,
    ENUM,
    CLASS,
    UNION,
    POINTER,
    REFERENCE,
    STD_STRING,
    STL_DEQUE,
    STL_SET,
    STL_MAP,
    STL_PAIR,
    CONST,
};

//! my custom container interface
class Container {
public:
    virtual ~Container() noexcept                 = default;
    virtual string serialize() const              = 0;
    virtual void   deserialize(const string_view) = 0;
};

//! my custom pair interface
class KeyValue {
public:
    ~KeyValue() noexcept                          = default;
    virtual string serialize() const              = 0;
    virtual void   deserialize(const string_view) = 0;
};

//! @brief get container type code structur
template<typename T> struct ContainerCode {
    static constexpr Keyword VALUE = Keyword::UNREGISTERED;
};

//! @brief unused type
enum class Registered : bool {
    UNREGISTERED = 0,
    REGISTERED   = 1
};

template<typename T> constexpr Keyword typecode();          //!< get type code
constexpr const char*                  typestring(Keyword); //!< reflect type name string by enum

template<typename T> constexpr bool isSTL();                        //!< check container explicit
template<typename T> constexpr bool isSTL(const T&);                //!< check container implicit
template<> bool                     isSTL<Keyword>(const Keyword&); //!< check container runtime

template<typename T> constexpr bool isOBJ();         //!< check object explicit
template<typename T> constexpr bool isOBJ(const T&); //!< check object implicit

/**
 * Key-Value Pair check, std::pair IS NOT KVP
 */
template<typename T> constexpr bool isKVP();                        //!< check pair explicit
template<typename T> constexpr bool isKVP(const T&);                //!< check pair implicit
template<> bool                     isKVP<Keyword>(const Keyword&); //!< check container runtime

//! @brief pre-registered metadata of T, return value is unused
template<typename T> Registered registclass();

//! @brief pre-registered metadata of T, return value is unused
template<typename T> Registered registenum();

//! @brief pre-registered methods of T, return value is unused
template<typename T> Registered registmethod();

} // namespace meta
LWE_END
#include "feature.ipp"
#endif
