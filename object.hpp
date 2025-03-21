#ifndef LWE_OBJECT_HEADER
#define LWE_OBJECT_HEADER

#include "meta.hpp"
#include "pool.hpp"

LWE_BEGIN

/**
 * @brief serializable object: template, array not support
 */
class Object {
    template<typename T> friend T*   create();
    template<typename T> friend void destroy(T*);

public:
    virtual ~Object();

public:
    virtual Class* meta() const;

public:
    std::string stringfy() const;
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
};

struct ObjectMeta: Class {
    virtual const char*      name() const override;
    virtual size_t           size() const override;
    virtual const Structure& fields() const override;
    virtual Class*           base() const override;
};

template<typename T> T*   create();
template<typename T> void destroy(T*);

LWE_END
#endif