#ifndef WINDOW_EXCEPTION_HPP_
#define WINDOW_EXCEPTION_HPP_
#include <Win32BaseException.hpp>

class WindowException : public Win32BaseException {
public:
	WindowException(int line, const char* file, long hr) noexcept;

	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	void GenerateWhatBuffer() noexcept override;
};

class NoGfxException : public Exception {
public:
	using Exception::Exception;
	const char* GetType() const noexcept override;
};

#endif