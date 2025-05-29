#ifdef LWE_SYS_ALERT

LWE_BEGIN
namespace diag {

template<typename Code, typename>
Alert::Alert(Code code, const std::error_category& category) noexcept : Alert(category.message(code), code) {}

Alert::Alert(const char* msg, int32 code) noexcept: Alert(string{ msg }, code) {}

Alert::Alert(const string& msg, int32 code) noexcept: code(code) {
	message = std::format(FORMAT, code);
	offset = message.length();
	message += string{ msg };
}

Alert::Alert(const Alert& in) noexcept: message(in.message), code(in.code), offset(in.offset) { }

Alert::Alert(Alert&& in) noexcept: code(in.code), offset(in.offset) {
	message = std::move(in.message);
}

Alert& Alert::operator=(const Alert& in) noexcept {
	message = in.message;
	code    = in.code;
	offset  = in.offset;
	return *this;
}

Alert& Alert::operator=(Alert&& in) noexcept {
	if (this != &in) {
		message = std::move(in.message);
		code    = in.code;
		offset  = in.offset;
	}
	return *this;
}

const char* Alert::what() const {
	return message.c_str();
}

const char* Alert::operator*() const {
	return what() + offset;
}

Alert::operator int() const {
	return code;
}

Alert::operator std::runtime_error() const {
	return std::runtime_error(message);
}

}
LWE_END

#endif