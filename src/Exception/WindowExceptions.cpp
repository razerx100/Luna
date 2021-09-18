#include <WindowExceptions.hpp>
#include <CleanWin.hpp>
#include <sstream>

WindowException::WindowException(int line, const char* file, long hr) noexcept
	: Win32BaseException(line, file, hr) {
	GenerateWhatBuffer();
}

void WindowException::GenerateWhatBuffer() noexcept {
	std::ostringstream oss;
	oss << GetType() << "\n"
		<< "[Error Code] " << GetErrorCode() << "\n"
		<< "[Description] " << GetErrorString() << "\n"
		<< GetOriginString();
	m_whatBuffer = oss.str();
}

const char* WindowException::what() const noexcept {
	return m_whatBuffer.c_str();
}

const char* WindowException::GetType() const noexcept {
	return "Window Exception";
}

const char* NoGfxException::GetType() const noexcept {
	return "Window Exception [No Graphics]";
}

void ExceptionMessageBox(
	const std::string& exceptionDetails,
	const std::string& exceptionType
) {
	MessageBoxA(
		nullptr, exceptionDetails.c_str(), exceptionType.c_str(),
		MB_OK | MB_ICONEXCLAMATION
	);
}
