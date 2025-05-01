#ifndef LWE_SERIALIZE
#define LWE_SERIALIZE


#include "object.hpp"

LWE_BEGIN
namespace meta {

template<typename T> string serialize(const T&);                    //!< primitive type to string
template<> string           serialize<bool>(const bool&);           //!< boolean type to string
template<> string           serialize<string>(const string&);       //!< string to string
template<> string           serialize<Container>(const Container&); //!< container to string

template<typename T> T deserialize(const string&);             //!< string to primitive type
template<> bool        deserialize<bool>(const string&);       //!< string to boolean type
template<> string      deserialize<string>(const string&);     //!< string to string
void                   deserialize(Container*, const string&); //!< string to container

void serialize(string*, const void*, const EType&);   // runtime parse
void deserialize(void*, const string&, const EType&); // runtime stringify

}
LWE_END
#endif