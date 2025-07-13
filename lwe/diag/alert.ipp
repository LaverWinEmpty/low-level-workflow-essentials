#ifdef LWE_SYS_ALERT

LWE_BEGIN
namespace diag {

template<typename Code, typename>
Alert::Alert(Code code, const std::error_category& category) noexcept: Alert(category.message(code), code) { }

Alert::Alert(const char* msg, int32_t code) noexcept: Alert(String{ msg }, code) { }

Alert::Alert(const String& msg, int32_t code) noexcept: code(code) {
    char buffer[FORMAT_SIZE + 1];
    std::snprintf(buffer, sizeof(buffer), FORMAT, code);

    message += buffer;
    message += msg;
}

Alert::Alert(const Alert& in) noexcept: message(in.message), code(in.code) { }

Alert::Alert(Alert&& in) noexcept: code(in.code) {
    message = std::move(in.message);
}

Alert& Alert::operator=(const Alert& in) noexcept {
    message = in.message;
    code    = in.code;
    return *this;
}

Alert& Alert::operator=(Alert&& in) noexcept {
    if(this != &in) {
        message = std::move(in.message);
        code    = in.code;
    }
    return *this;
}

const char* Alert::what() const {
    return message.c_str();
}

const char* Alert::operator*() const {
    return what() + FORMAT_SIZE;
}

Alert::operator int() const {
    return code;
}

Alert::operator std::runtime_error() const {
    return std::runtime_error(message);
}

} // namespace diag
LWE_END

#endif
