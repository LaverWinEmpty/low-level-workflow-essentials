#ifdef LWE_SYS_ALERT

LWE_BEGIN
namespace diag {

template<typename Code, typename>
Alert::Alert(Code code, const std::error_category& category) noexcept : Alert(category.message(code), code) {}

Alert::Alert(const char* msg, int32_t code) noexcept: Alert(string{ msg }, code) {}

Alert::Alert(const string& msg, int32_t code) noexcept: code(code) {
	// use as buffer
	message.resize(FORMAT_SIZE + msg.size());
	std::sprintf(message.data(), FORMAT, code);
	std::memcpy(message.data() + FORMAT_SIZE, msg.c_str(), msg.size());
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
	if (this != &in) {
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

}
LWE_END

#endif