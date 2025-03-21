#ifndef LWE_SERIALIZE_HEADER
#define LWE_SERIALIZE_HEADER

#include <sstream>
#include <iomanip>
#include "object.hpp"

LWE_BEGIN

template<typename T> string serialize(const T&);                              //!< primitive type to string
template<> string           serialize<bool>(const bool&);                     //!< boolean type to string
template<> string           serialize<string>(const string&);                 //!< string to string
template<> string           serialize<stl::Container>(const stl::Container&); //!< container to string

template<typename T> T deserialize(const string&);                  //!< string to primitive type
template<> bool        deserialize<bool>(const string&);            //!< string to boolean type
template<> string      deserialize<string>(const string&);          //!< string to string
void                   deserialize(stl::Container*, const string&); //!< string to container

void serialize(string*, const void*, const EType&);   // runtime parse
void deserialize(void*, const string&, const EType&); // runtime stringify

LWE_END
#endif