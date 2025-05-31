#ifndef LWE_OBJECT
#define LWE_OBJECT

#include "type.hpp"
#include "feature.hpp"

#include "../../mem/pool.hpp"

LWE_BEGIN
namespace meta {

struct Class; // meta class

/**
 * @brief serializable object: template, array not support
 */
class Object {
    template<typename T> friend T*   create();
    template<typename T> friend void destroy(T*);
    friend Object*                   create(const Class*);
    friend void                      destroy(Object*);

public:
    virtual ~Object();

public:
    virtual Class* meta() const;

public:
    std::string stringify() const;
    void        parse(const std::string& in);
    static void parse(Object* out, const std::string& in);

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
    static sync::Lock lock;
};

} // namespace meta
LWE_END
#endif