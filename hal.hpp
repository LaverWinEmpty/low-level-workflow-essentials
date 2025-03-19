#ifndef LWE_HAL_HEADER
#define LWE_HAL_HEADER

#ifdef _POSIX_VERSION
#    include <sys/types.h>
#endif
#include <cstdlib>
#include <cstdint>
#include <type_traits>
#include <optional>

#include "std.hpp"
#include "macro.h"

LWE_BEGIN
namespace HAL {

using int8    = int8_t;        //!< signed
using int16   = int16_t;       //!< signed
using int32   = int32_t;       //!< signed
using int64   = int64_t;       //!< signed
using uint8   = uint8_t;       //!< unsigned
using uint16  = uint16_t;      //!< unsigned
using uint32  = uint32_t;      //!< unsigned
using uint64  = uint64_t;      //!< unsigned
using fast8   = int_fast8_t;   //!< fast signed
using fast16  = int_fast16_t;  //!< fast signed
using fast32  = int_fast32_t;  //!< fast signed
using fast64  = int_fast64_t;  //!< fast signed
using ufast8  = uint_fast8_t;  //!< fast unsigned
using ufast16 = uint_fast16_t; //!< fast unsigned
using ufast32 = uint_fast32_t; //!< fast unsigned
using ufast64 = uint_fast64_t; //!< fast unsigned
using float32 = float;         //!< floating
using float64 = double;        //!< floating
using ptrdiff = ptrdiff_t;     //!< pointer difference
using uintptr = uintptr_t;     //!< pointer unsigned
using intptr  = intptr_t;      //!< pointer signed
using byte    = uint8;         //!< unsigned byte type

using size_t  = std::size_t;                //!< size type
using hash_t  = std::size_t;                //!< hash type
using index_t = std::make_signed_t<size_t>; //!< index type

#ifdef _POSIX_VERSION
using ssize_t = ::ssize_t;
#else
using ssize_t = std::make_signed_t<size_t>;
#endif

} // namespace HAL
LWE_END

using namespace LWE::HAL;

/**************************************************************************************************
 * using standard library
 **************************************************************************************************/

using std::function;
using std::string;

using std::nullopt;
using std::nullopt_t;
template<typename T> using opt = std::optional<T>;
#endif
