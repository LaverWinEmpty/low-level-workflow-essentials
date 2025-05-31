#ifndef LWE_META_SERIALIZE
#define LWE_META_SERIALIZE
LWE_BEGIN
namespace meta {

template<typename T> string serialize(const T&);                    //!< primitive type to string
template<> string           serialize<bool>(const bool&);           //!< boolean type to string
template<> string           serialize<string>(const string&);       //!< string to string
template<> string           serialize<Container>(const Container&); //!< container to string
template<typename T> string serialize(const Container&);            //!< container to string, T is derived type

template<typename T> T    deserialize(const string&);             //!< string to primitive type
template<> bool           deserialize<bool>(const string&);       //!< string to boolean type
template<> string         deserialize<string>(const string&);     //!< string to string
void                      deserialize(Container*, const string&); //!< string to container
template<typename T> void deserialize(Container*, const string&); //!< string to container

void serialize(string*, const void*, const Keyword&);   //!< runtime serialize
void deserialize(void*, const string&, const Keyword&); //!< runtime deserialize

}
LWE_END
#endif