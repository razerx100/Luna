#ifndef WIN32_BASE_EXCEPTION_HPP_
#define WIN32_BASE_EXCEPTION_HPP_
#include <Exception.hpp>

class Win32BaseException : public Exception {
public:
	Win32BaseException(int line, const char* file, long hr) noexcept;

	static std::string TranslateErrorCode(long hr) noexcept;
	[[nodiscard]]
	long GetErrorCode() const noexcept;
	[[nodiscard]]
	std::string GetErrorString() const noexcept;

private:
	long m_hr;
};
#endif
