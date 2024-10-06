#ifndef WINDOW_EXCEPTION_HPP_
#define WINDOW_EXCEPTION_HPP_
#include <Exception.hpp>

class WindowException final : public Exception
{
public:
	WindowException(std::int32_t line, std::string file, long hr);

private:
	[[nodiscard]]
	const char* GetType() const noexcept { return "Window Exception"; }
	[[nodiscard]]
	std::string GetErrorString() const noexcept { return TranslateErrorCode(m_hr); }

	void GenerateWhatBuffer() noexcept;

	[[nodiscard]]
	static std::string TranslateErrorCode(long hr) noexcept;

private:
	long m_hr;
};
#endif