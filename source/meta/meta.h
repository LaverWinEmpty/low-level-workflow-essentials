#ifndef LWE_META
#define LWE_META

/**************************************************************************************************
 * meta headers
 **************************************************************************************************/
#include "internal/registry.hpp" // has static variable
#include "internal/feature.hpp"  // include type, object, container

/**************************************************************************************************
 * meta sources (beware of dependencies)
 **************************************************************************************************/
#include "internal/type.ipp"
#include "internal/object.ipp"
#include "internal/container.ipp"
#include "internal/feature.ipp"

LWE_BEGIN
/**************************************************************************************************
 * external exposure
 **************************************************************************************************/
using meta::Type;        //!< type: field type info
using meta::Object;      //!< type: object class
using meta::Class;       //!< type: meta class
using meta::Enum;        //!< type: meta enum
using meta::Container;   //!< type: container
using meta::create;      //!< function: new object
using meta::destroy;     //!< function: delete object
using meta::serialize;   //!< function: serialize
using meta::deserialize; //!< function: deserialize
using meta::typeof;      //!< function: get type info
using meta::classof;     //!< function: get class metadata
using meta::enumof;      //!< function: get enum metadata
using meta::statics;     //!< function: get static object
using meta::typestring;  //!< function: get typename
using meta::method;      //!< function: get method

/**************************************************************************************************
 * register type enum
 **************************************************************************************************/
REGISTER_ENUM_BEGIN(Keyword, meta) {
    REGISTER_ENUM(UNREGISTERED);
    REGISTER_ENUM(VOID);
    REGISTER_ENUM(SIGNED_INT);
    REGISTER_ENUM(SIGNED_CHAR);
    REGISTER_ENUM(SIGNED_SHORT);
    REGISTER_ENUM(SIGNED_LONG);
    REGISTER_ENUM(SIGNED_LONG_LONG);
    REGISTER_ENUM(UNSIGNED_SHORT);
    REGISTER_ENUM(UNSIGNED_INT);
    REGISTER_ENUM(UNSIGNED_CHAR);
    REGISTER_ENUM(UNSIGNED_LONG);
    REGISTER_ENUM(UNSIGNED_LONG_LONG);
    REGISTER_ENUM(BOOL);
    REGISTER_ENUM(CHAR);
    REGISTER_ENUM(WCHAR_T);
    REGISTER_ENUM(FLOAT);
    REGISTER_ENUM(DOUBLE);
    REGISTER_ENUM(LONG_DOUBLE);
    REGISTER_ENUM(ENUM);
    REGISTER_ENUM(CLASS);
    REGISTER_ENUM(UNION);
    REGISTER_ENUM(POINTER);
    REGISTER_ENUM(REFERENCE);
    REGISTER_ENUM(FUNCTION);
    REGISTER_ENUM(STD_STRING);
    REGISTER_ENUM(STL_DEQUE);
    REGISTER_ENUM(CONST);
}
REGISTER_ENUM_END;
LWE_END
#endif