#ifndef WINDOW_EXCEPTION_HPP_
#define WINDOW_EXCEPTION_HPP_
#include <Win32BaseException.hpp>

class WindowException final : public Win32BaseException {
public:
	WindowException(int line, const char* file, long hr) noexcept;

	[[nodiscard]]
	const char* what() const noexcept override;
	[[nodiscard]]
	const char* GetType() const noexcept override;
	void GenerateWhatBuffer() noexcept override;
};

class NoGfxException final : public Exception {
public:
	using Exception::Exception;

	[[nodiscard]]
	const char* GetType() const noexcept override;
};

#endif