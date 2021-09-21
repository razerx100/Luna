#include <WindowExceptions.hpp>
#include <CleanWin.hpp>
#include <sstream>
#include <ExceptionMessageBox.hpp>

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
	const char* exceptionDetails,
	const char* exceptionType
) {
	MessageBoxA(
		nullptr, exceptionDetails, exceptionType,
		MB_OK | MB_ICONEXCLAMATION
	);
}
