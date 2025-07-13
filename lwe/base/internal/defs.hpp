#ifndef LWE_CORE_ALIAS
#define LWE_CORE_ALIAS

#ifdef _POSIX_VERSION
#    include <sys/types.h>
#endif

#include "std.hpp"
#include "macro.hpp"

LWE_BEGIN
namespace core {

#ifdef _POSIX_VERSION
using ssize_t = ::ssize_t;
#else
using ssize_t = std::make_signed_t<size_t>;
#endif

using uint = unsigned int;

using hash_t  = uint64_t;                   //!< 64-bit hash type
using index_t = std::make_signed_t<size_t>; //!< index type

} // namespace core
LWE_END

#endif
