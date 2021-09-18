#ifndef __WIN32_BASE_EXCEPTION_HPP__
#define __WIN32_BASE_EXCEPTION_HPP__
#include <Exception.hpp>

class Win32BaseException : public Exception {
public:
	Win32BaseException(int line, const char* file, long hr) noexcept;

	static std::string TranslateErrorCode(long hr) noexcept;
	long GetErrorCode() const noexcept;
	std::string GetErrorString() const noexcept;

private:
	long m_hr;
};
#endif
