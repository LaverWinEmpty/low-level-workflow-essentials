#ifndef LWE_SERIALIZE_HEADER
#define LWE_SERIALIZE_HEADER

#include <sstream>
#include <iomanip>
#include "meta.hpp"

template<typename T> string tostr(const T&);                                        //!< primitive type to string
template<> string           tostr<bool>(const bool&);                               //!< boolean type to string
template<> string           tostr<string>(const string&);                           //!< string to string
template<> string           tostr<LWE::stl::Container>(const LWE::stl::Container&); //!< container to string

template<typename T> T fromstr(const string&);                       //!< string to primitive type
template<> bool        fromstr<bool>(const string&);                 //!< string to boolean type
template<> string      fromstr<string>(const string&);               //!< string to string
void                   fromstr(LWE::stl::Container*, const string&); //!< string to container

void tostr(std::string*, const void*, const MetaType&);   // runtime deserialize
void fromstr(void*, const std::string&, const MetaType&); // runtime serialize

#include "serialize.ipp"
#endif