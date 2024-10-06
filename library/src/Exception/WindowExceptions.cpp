#include <WindowExceptions.hpp>
#include <CleanWin.hpp>
#include <ExceptionMessageBox.hpp>
#include <format>

WindowException::WindowException(std::int32_t line, std::string file, long hr)
	: Exception{ line, std::move(file) }, m_hr{ hr }
{
	GenerateWhatBuffer();
}

void WindowException::GenerateWhatBuffer() noexcept
{
	m_whatBuffer = std::format(
		"{}\n[Error Code] {}\n[Description] {}\n{}",
		GetType(), m_hr, GetErrorString(), GetOriginString()
	);
}

std::string WindowException::TranslateErrorCode(long hr) noexcept
{
	LPSTR msgBuffer = nullptr;

	DWORD msgLength = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, static_cast<DWORD>(hr), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&msgBuffer), 0, nullptr
	);

	if (!msgLength)
		return "Unidentified error code";

	std::string errorString = msgBuffer;

	LocalFree(msgBuffer);

	return errorString;
}

void ExceptionMessageBox(const char* exceptionDetails, const char* exceptionType
) {
	MessageBox(nullptr, exceptionDetails, exceptionType, MB_OK | MB_ICONEXCLAMATION);
}
