#ifndef LWE_OBJECT_HEADER
#define LWE_OBJECT_HEADER

#include "meta.hpp"

LWE_BEGIN

//! @brief
class Object {
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
};

struct ObjectMeta: Class {
    virtual const char*      name() const override;
    virtual size_t           size() const override;
    virtual const Structure& fields() const override;
    virtual Class*           base() const override;
};

LWE_END
#endif