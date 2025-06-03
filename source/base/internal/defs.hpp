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

using byte_t    = uint8_t;       //!< byte unsigned
using word_t    = signed int;    //!< signed
using ubyte_t   = unsigned int;  //!< unsigned
using fast8_t   = int_fast8_t;   //!< fast signed
using fast16_t  = int_fast16_t;  //!< fast signed
using fast32_t  = int_fast32_t;  //!< fast signed
using fast64_t  = int_fast64_t;  //!< fast signed
using ufast8_t  = uint_fast8_t;  //!< fast unsigned
using ufast16_t = uint_fast16_t; //!< fast unsigned
using ufast32_t = uint_fast32_t; //!< fast unsigned
using ufast64_t = uint_fast64_t; //!< fast unsigned
using float32_t = float;         //!< floating
using float64_t = double;        //!< floating

using hash_t  = std::size_t;                //!< hash type
using index_t = std::make_signed_t<size_t>; //!< index type

} // namespace hal
LWE_END


#endif
