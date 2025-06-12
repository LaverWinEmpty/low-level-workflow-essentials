#ifndef LWE_OBJECT
#define LWE_OBJECT

#include "type.hpp"
#include "lambda.hpp"

#include "../mem/pool.hpp"

LWE_BEGIN
namespace meta {

struct Class; // meta class

/**
 * @brief serializable object: template, array not support
 */
class Object {
    template<typename T> friend class Ref;

protected:
    template<typename T> static T*   constructor();
    template<typename T> static void destructor(T*);
    static Object*                   constructor(const Class*);
    static void                      destructor(Object*);

public:
    virtual ~Object();

public:
    virtual Class* meta() const;

public:
    std::string serialize() const;
    void        deserialize(const std::string& in);
    static void deserialize(Object* out, const std::string& in);

public:
    template<typename T> bool isof() const;              //!< check same type of derived by template
    bool                      isof(const Class*) const;  //!< check same type of derived by meta class
    bool                      isof(const char*) const;   //!< check same type of derived by name
    bool                      isof(const string&) const; //!< check same type of derived by name

private:
    struct Pool {
        ~Pool() {
            for(auto& p : map) delete p.second;
        }
        std::unordered_map<size_t, mem::Pool*> map;
    };
    static std::unordered_map<size_t, mem::Pool*>& pool() {
        static std::unordered_map<size_t, mem::Pool*> instance;
        return instance;
    }
    static async::Lock lock;
};

//! @brief Object metadata, has not base -> manual generation
struct ObjectMeta: Class {
    virtual const char*      name() const override;
    virtual size_t           size() const override;
    virtual const Structure& fields() const override;
    virtual const Class*     base() const override;
    virtual const Object*    statics() const override;
    virtual Object*          construct(Object*) const override;
};

template<typename T> string serialize(const T&);                    //!< primitive type to string
template<> string           serialize<bool>(const bool&);           //!< boolean type to string
template<> string           serialize<string>(const string&);       //!< string to string
template<> string           serialize<Container>(const Container&); //!< container to string
template<typename T> string serialize(const Container*);            //!< container to string, T is derived type

template<typename T> T    deserialize(const string&);             //!< string to primitive type
template<> bool           deserialize<bool>(const string&);       //!< string to boolean type
template<> string         deserialize<string>(const string&);     //!< string to string
void                      deserialize(Container*, const string&); //!< string to container
template<typename T> void deserialize(Container*, const string&); //!< string to container, T is dervied type

void serialize(string*, const void*, const Keyword&);   //!< runtime serialize
void deserialize(void*, const string&, const Keyword&); //!< runtime deserialize

} // namespace meta
LWE_END

#include "object.ipp"
#include "internal/rc.hpp"
#endif