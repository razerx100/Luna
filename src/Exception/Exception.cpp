#include <Exception.hpp>
#include <sstream>

Exception::Exception(int line, const char* file) noexcept
		: m_line(line), m_file(file) {}

const char* Exception::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << "\n"
		<< GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* Exception::GetType() const noexcept {
	return "Exception";
}

int Exception::GetLine() const noexcept {
	return m_line;
}

const std::string& Exception::GetFile() const noexcept {
	return m_file;
}

std::string Exception::GetOriginString() const noexcept {
	std::ostringstream oss;
	oss << "[File] " << m_file << "\n"
		<< "[Line] " << m_line;

	return oss.str();
}

GenericException::GenericException(const std::string& errorText) noexcept
	: m_errorText(errorText) {}

const char* GenericException::what() const noexcept {
	return m_errorText.c_str();
}
