#ifndef LWE_OBJECT
#define LWE_OBJECT

#include "type.hpp"
#include "lambda.hpp"

#include "../mem/pool.hpp"
#include <type_traits>

LWE_BEGIN
namespace meta {

struct Class; // meta class

/**
 * @brief serializable object: template, array not support
 */
class Object {
    template<typename T> friend class RC;

protected:
    template<typename T> static T*   constructor();
    template<typename T> static void destructor(T*);
    static Object*                   constructor(const Class*);
    static void                      destructor(Object*);

public:
    virtual ~Object() = default;

public:
    virtual Class* meta() const;

public:
    std::string serialize() const;
    void        deserialize(const string_view in);
    static void deserialize(Object* out, const string_view in);

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

} // namespace meta
LWE_END

#include "object.ipp"
#include "internal/rc.hpp"
#endif
