#ifndef LWE_META_FEATURE
#define LWE_META_FEATURE

#include "registry.hpp"
#include "reflector.hpp"

LWE_BEGIN
namespace meta {

//! @brief type codes
enum class Keyword : uint8_t {
    UNREGISTERED, // TODO: STRUCT
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
    STD_PAIR,
    STL_STACK,
    STL_DEQUE,
    STL_SET,
    STL_MAP,
    STL_ANY,
    CONST,
};

constexpr bool storable(Keyword in) {
    switch(in) {
        case lwe::meta::Keyword::STL_STACK: return true;
        case lwe::meta::Keyword::STL_DEQUE: return true;
        case lwe::meta::Keyword::STL_SET:   return true;
        case lwe::meta::Keyword::STL_MAP:   return true;
        default:                            return false;
    }
}

// TODO: Container / KeyCalue to Encoder
//! @brief interface: callable in `Codec`
class Encoder {
public:
    virtual ~Encoder() noexcept                  = default;
    virtual String serialize() const             = 0;
    virtual void   deserialize(const StringView) = 0;
};
//! @brief container type eraser
class Container: public Encoder { };

////! my custom container interface
// class Container {
// public:
//    virtual ~Container() noexcept                = default;
//    virtual String serialize() const             = 0;
//    virtual void   deserialize(const StringView) = 0;
//};
//
////! my custom pair interface
// class KeyValue {
// public:
//    ~KeyValue() noexcept                         = default;
//    virtual String serialize() const             = 0;
//    virtual void   deserialize(const StringView) = 0;
//};

//! @brief get container type code structur
template<typename T> struct ContainerCode {
    static constexpr Keyword VALUE = Keyword::UNREGISTERED;
};

//! @brief type eraser default
template<typename T, typename = void>
struct TypeEraser {
    static constexpr auto KEYWORD = Keyword::UNREGISTERED;
};
// pair
template<typename T>
struct TypeEraser<T, std::void_t<typename T::first_type, typename T::second_type>> {
    static constexpr auto KEYWORD = Keyword::STD_PAIR;
};

//! @brief unused type
enum class Registered : bool {
    UNREGISTERED = 0,
    REGISTERED   = 1
};

template<typename T> constexpr Keyword typecode();          //!< get type code
constexpr const char*                  typestring(Keyword); //!< reflect type name string by enum

// template<typename T> constexpr bool isSTL();                        //!< check container explicit
// template<typename T> constexpr bool isSTL(const T&);                //!< check container implicit
// template<> bool                     isSTL<Keyword>(const Keyword&); //!< check container runtime
// template<typename T> constexpr bool isOBJ();         //!< check object explicit
// template<typename T> constexpr bool isOBJ(const T&); //!< check object implicit
// template<typename T> constexpr bool isKVP();                        //!< check pair explicit
// template<typename T> constexpr bool isKVP(const T&);                //!< check pair implicit
// template<> bool                     isKVP<Keyword>(const Keyword&); //!< check container runtime

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
