#ifndef LWE_DIAG_HEADER
#define LWE_DIAG_HEADER

#include "alert.hpp"
#include "expected.hpp"
#include "log.hpp"

#define ERROR_BODY(X)                                    \
   X(SUCCESS)       /* not error                      */ \
   X(BAD_ALLOC)     /* bad alloc                      */ \
   X(OUT_OF_RANGE)  /* out of range                   */ \
   X(TYPE_MISMATCH) /* bad any cast, format error ... */ \
   X(INVALID_DATA)  /* null, not found, already ...   */ \
   X(UNKNOWN_ERROR) /*                                */

#define ERROR_USING(NAME) static constexpr int32_t NAME = Code::##NAME;

#define REGISTER_ERROR_CODE(NAME)    NAME,
#define REGISTER_ERROR_MESSAGE(NAME) #NAME,
#define REGISTER_ERROR_USING(NAME)   constexpr Code NAME = Code::NAME;

LWE_BEGIN 
namespace diag {

enum class Code : int32_t {
	ERROR_BODY(REGISTER_ERROR_CODE)
};
ERROR_BODY(REGISTER_ERROR_USING);

Alert error(int32_t code) {
	static constexpr const char* ERROR_MESSAGE[] = {
		ERROR_BODY(REGISTER_ERROR_MESSAGE)
	};
	static constexpr int32_t ERROR_COUNT = (sizeof(ERROR_MESSAGE) / sizeof(*ERROR_MESSAGE));

	// out of range
	if (code < 0 || code >= ERROR_COUNT) {
		code = ERROR_COUNT - 1;
	}
	return Alert(ERROR_MESSAGE[code], code);
}

Alert error(Code code) {
	return error(static_cast<int32_t>(code));
}

}
LWE_END
#endif