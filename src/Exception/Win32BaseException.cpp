#include <Win32BaseException.hpp>
#include <CleanWin.hpp>

Win32BaseException::Win32BaseException(int line, const char* file, long hr) noexcept
	: Exception(line, file), m_hr(hr) {}

std::string Win32BaseException::TranslateErrorCode(long hr) noexcept {
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

long Win32BaseException::GetErrorCode() const noexcept {
	return m_hr;
}

std::string Win32BaseException::GetErrorString() const noexcept {
	return TranslateErrorCode(m_hr);
}
