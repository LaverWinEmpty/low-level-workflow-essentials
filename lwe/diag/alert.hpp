#ifndef LWE_SYS_ALERT
#define LWE_SYS_ALERT

#include "../base/base.h"

/*
#include <stdexcept>
#include <system_error>
#include <format>
#include <string>
*/

LWE_BEGIN
namespace diag {

class Alert: public std::exception {
    static constexpr const char* FORMAT = "[0x%08X] "; //!< "FORMAT: error message"

private:
    //! @brief formatting buffer size
    //! @note  buffer size = format size - '\0' - %08X size + FFFFFFFF size
    static constexpr const size_t FORMAT_SIZE = 10 - 1 - 4 + 8; //!< 13 ("[0xFFFFFFFF] ")

public:
    /// @brief not exception
    Alert() noexcept = default;

public:
    /// @brief get system error
    /// @note  SFINAE: 0 is not nullptr
    /// @param [in] T error code
    /// @param [in] std::error_category
    template<typename Code, typename = std::enable_if_t<!std::is_pointer_v<Code>>>
    Alert(Code, const std::error_category& = std::system_category()) noexcept;

public:
    /// @brief get custom error
    /// @param [in] string error message
    /// @param [in] int32  error code
    Alert(const char*, int32_t = -1) noexcept;

public:
    /// @brief get custom error
    /// @param [in] char* error message
    /// @param [in] int32 error code
    Alert(const String&, int32_t = -1) noexcept;

public:
    /// @brief copy
    Alert(const Alert&) noexcept;

public:
    /// @brief move
    Alert(Alert&&) noexcept;

public:
    /// @brief copy
    Alert& operator=(const Alert&) noexcept;

    /// @brief move
    Alert& operator=(Alert&&) noexcept;

public:
    /// @brief override, get code + message string
    const char* what() const;

public:
    /// @brief get message only
    const char* operator*() const;

public:
    /// @brief get code
    operator int() const;

public:
    /// @brief get to runtime error
    operator std::runtime_error() const;

private:
    String  message = "";
    int32_t code    = 0;
    // size_t  offset  = 0; //!< message begin pos
};

} // namespace diag
LWE_END
#include "alert.ipp"
#endif
