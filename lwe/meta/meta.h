#ifndef LWE_META_HEADER
#define LWE_META_HEADER

/**************************************************************************************************
 * meta header description
 **************************************************************************************************
 * simple UML
 * ! < ^ > (arrow) is reference, else composition or inheritance, specialization
 * ! `<=[ ]` description
 *
 *                               +-----------+
 *                               | Container | <=[container type]
 *                               +-----------+
 *                                     ^
 *                                     |
 *                                +----+----+
 *                                | Keyword | <=[enum(VOID, INT, CONST ...)]
 *                                +----+----+
 *                                     |
 *                                 +---+---+
 *                                 | Type  | <=[type info: (Keyword array]
 *                                 +---+---+
 *                                     |
 *         +---------------------------+------------------------------+
 *         |                           |                              |
 *  +------+------+                +---+---+                    +-----+-----+
 *  | Enumerator  | <=[enum info]  | Field | <=[class info]     | Signature | <=[method info]
 *  +------+------+                +---+---+                    +-----+-----+
 *         |                           |                              |
 *         +--------------+------------+                         +----+----+
 *                        |                                      | Method  | <=[`Lambda` base]
 *                +-------+------+                               +----+----+
 *                | Reflector<T> | <=[T array, CRTP base]             |
 *                +-------+------+                              +-----+-----+
 *                        |                                     | Lambda<T> | <=[reflection]
 *         +--------------+--------------+                      +-----------+
 *         |                             |                            ^
 *   +-----+-----+                +------+------+                     |
 *   | Structure | <=[<Field>]    | Enumeration | <=[<Enumerator>]    |
 *   +-----+-----+                +------+------+                     |
 *         |                             |                            |
 *     +---+---+                     +---+---+                        |
 *     | Class | <=[class metadata]  | Enum  | <=[enum metadata]      |
 *     +-------+                     +-------+                        |
 *         ^                             ^                            |
 *         |                             |                            |
 *         |                             +----------------------------+
 *    +----+----+                        |                            |
 *    | Object  | <=[reflection]     +---+---+                  +-----+-----+
 *    +---------+                    | Value | <=[reflection]   | Registry  | <=[internal]
 *         ^                         +-------+                  +-----+-----+
 *         |                                                          |
 *         +----------------------------------------------------------+
 *
 * ! `Class`: metadata base of `Object`
 * ! `Enum`: metadata base of registered enums. referenced only when serializing.
 * ! `Value`: enum helper object, unregistered enums can also use it.
 * ! `Type` <-> `Container` <-> `Object`: cross-reference for type check in serialize
 * ! `Keyword` -> `Container` when type checking.
 **************************************************************************************************/

/**************************************************************************************************
 * config
 **************************************************************************************************/
#define REGISTER_ENUM_KEYWORD_FLAG 0

/**************************************************************************************************
 * include
 **************************************************************************************************/
#include "type.hpp"
#include "value.hpp"  // enum helper
#include "lambda.hpp" // included type.hpp
#include "object.hpp" // included lambda.hpp
#include "codec.hpp"  // for serialize

LWE_BEGIN
/**************************************************************************************************
 * external exposure (type)
 **************************************************************************************************/
using meta::Class;     //!< meta class
using meta::Container; //!< container
using meta::Enum;      //!< meta enum
using meta::Object;    //!< object class
using meta::Type;      //!< field type info
using meta::Value;     //!< enum helper

/**************************************************************************************************
 * external exposure (fucntion)
 **************************************************************************************************/
using meta::typeof;     //!< get type info
using meta::classof;    //!< get class metadata
using meta::enumof;     //!< get enum metadata
using meta::methodof;   //!< get method
using meta::statics;    //!< get static object
using meta::typestring; //!< get typename

/**************************************************************************************************
 * register type enum
 **************************************************************************************************/
#if defined(REGISTER_ENUM_KEYWORD_FLAG) && REGISTER_ENUM_KEYWORD_FLAG
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
#endif

LWE_END
#endif
