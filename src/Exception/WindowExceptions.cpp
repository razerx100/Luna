#include <WindowExceptions.hpp>
#include <sstream>

WindowException::WindowException(int line, const char* file, HRESULT hr) noexcept
	: Exception(line, file), m_hr(hr) {}

const char* WindowException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << "\n"
		<< "[Error Code] " << GetErrorCode() << "\n"
		<< "[Description] " << GetErrorString() << "\n"
		<< GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* WindowException::GetType() const noexcept {
	return "Window Exception";
}

const char* NoGfxException::GetType() const noexcept {
	return "Window Exception [No Graphics]";
}

std::string WindowException::TranslateErrorCode(long hr) noexcept {
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, static_cast<DWORD>(hr), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);

	if (!nMsgLen)
		return "Unidentified error code";

	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

long WindowException::GetErrorCode() const noexcept {
	return m_hr;
}

std::string WindowException::GetErrorString() const noexcept {
	return TranslateErrorCode(m_hr);
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
