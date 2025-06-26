#ifndef LWE_UTIL_ID
#define LWE_UTIL_ID

#include "../base/base.h"

LWE_BEGIN
namespace util {

template<typename T> class ID {
    static uint64_t next();

public:
    //! @brief set id 0
    ID(Uninit);

public:
    //! @brief set id generated
    ID();

public:
    bool operator==(const ID<T>&) const;
    bool operator!=(const ID<T>&) const;

public:
    //! @brief getter
    explicit operator uint64_t() const;

public:
    //! @brief getter
    uint64_t value() const;

public:
    //! @brief uninit -> lazy generate
    //! @note  ignore when id is not 0
    void gen();

private:
    uint64_t id;
};

//! @brief universal id
using UID = ID<void>;

} // namespace util
LWE_END
#include "id.ipp"
#endif
