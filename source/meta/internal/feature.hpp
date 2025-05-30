#ifndef LWE_META_FEATURE
#define LWE_META_FEATURE

#include "type.hpp"
#include "container.hpp"
#include "object.hpp"

LWE_BEGIN
namespace meta {

class  Object;
struct Class;
struct Enum;
class  Method;

template<typename T> T*   create();    //!< @breif create object
template<typename T> void destroy(T*); //!< @breif destroy object

//! @brief pre-registered metadata of typename T, return value is unused
template<typename T> Registered registclass();

//! @brief pre-registered metadata of typename T, return value is unused
template<typename T> Registered registenum();

//! @brief pre-registered method signature of typename T, return value is unused
template<typename T> Registered registmethod();

template<typename T> constexpr Keyword typecode(); //!< get type code

constexpr const char*            typestring(Keyword);     //!< reflect type name string by enum
template<typename T> const char* typestring();            //!< reflect type name string explicit
template<typename T> const char* typestring(const T&);    //!< reflect type name string implicit
const char*                      typestring(const Type&); //!< reflect type name

template<typename T> const Type& typeof();         //!< reflect typeinfo by template
template<typename T> const Type& typeof(const T&); //!< reflect typeinfo by argument
template<typename T> void        typeof(Type*);    //!< pirvate

template<typename T> Object* statics();              //!< get static class
template<typename T> Object* statics(const T&);      //!< get static class
Object*                      statics(const char*);   //!< get static class
Object*                      statics(const string&); //!< get static class

template<typename T> Class* classof();              //!< get class field list
template<typename T> Class* classof(const T&);      //!< get class field list
Class*                      classof(const char*);   //!< get class field list
Class*                      classof(const string&); //!< get class field list

template<typename T> Enum* enumof();              //!< get enum value list
template<typename T> Enum* enumof(const T&);      //!< get enum value list
Enum*                      enumof(const char*);   //!< get enum value list
Enum*                      enumof(const string&); //!< get enum value list

template<typename T> Method* method(const char*   name);                    //!< get method
template<typename T> Method* method(const string& name);                    //!< get method
Method*                      method(const char*   cls, const char*   name); //!< get method
Method*                      method(const char*   cls, const string& name); //!< get method
Method*                      method(const string& cls, const char*   name); //!< get method
Method*                      method(const string& cls, const string& name); //!< get method

template<typename T> string serialize(const T&);                    //!< primitive type to string
template<> string           serialize<bool>(const bool&);           //!< boolean type to string
template<> string           serialize<string>(const string&);       //!< string to string
template<> string           serialize<Container>(const Container&); //!< container to string

template<typename T> T deserialize(const string&);             //!< string to primitive type
template<> bool        deserialize<bool>(const string&);       //!< string to boolean type
template<> string      deserialize<string>(const string&);     //!< string to string
void                   deserialize(Container*, const string&); //!< string to container

void serialize(string*, const void*, const Keyword&);   // runtime parse
void deserialize(void*, const string&, const Keyword&); // runtime stringify

template<typename T> constexpr bool isSTL();                    //!< check container explicit
template<typename T> constexpr bool isSTL(const T&);            //!< check container implicit
template<> bool                     isSTL<Keyword>(const Keyword&); //!< check container type code

}
LWE_END
#endif