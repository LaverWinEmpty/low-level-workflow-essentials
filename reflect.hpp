#ifndef LWE_REFLECT
#define LWE_REFLECT

#include "meta/meta.ipp"
#include "meta/object.ipp"
#include "meta/serialize.ipp"
#include "meta/container.ipp"

#include "meta/meta.inl"
#include "meta/object.inl"
#include "meta/serialize.inl"
#include "meta/container.inl"

LWE_BEGIN
/*
 * @brief object class
 */
    using meta::Object;

/*
 * @brief meta class
 */
using meta::Class;

/*
 * @brief meta enum
 */
using meta::Enum;

/*
 * @brief field type info
 */
using meta::Type;

/*
 * @brief new object
 */
using meta::create;

/*
 * @brief delete object
 */
 using meta::destroy;

/*
 * @brief serialize
 */
using meta::serialize;

/*
 * @brief deserialize
 */
using meta::deserialize;

/*
 * @brief get class metadata
 */
using meta::classof;

/*
 * @brief get class metadata
 */
using meta::enumof;

/*
 * @brief get static class
 */
using meta::statics;

/*
 * @brief get typename
 */
using meta::typestring;

REGISTER_ENUM_BEGIN(EType, meta) {
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