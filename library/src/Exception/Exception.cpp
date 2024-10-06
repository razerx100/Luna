#include <Exception.hpp>
#include <format>

Exception::Exception(std::int32_t line, std::string file)
	: m_line{ line }, m_file{ std::move(file) }, m_whatBuffer{}
{
	GenerateWhatBuffer();
}

void Exception::GenerateWhatBuffer() noexcept
{
	m_whatBuffer = std::format("{}\n{}", GetType(), GetOriginString());
}

std::string Exception::GetOriginString() const noexcept
{
	return std::format("[File] {}\n[Line] {}", m_file, m_line);
}

GenericException::GenericException(std::int32_t line, std::string file, std::string errorText)
	: Exception{ line, std::move(file) }, m_errorText{ std::move(errorText) }
{
	GenerateWhatBuffer();
}

void GenericException::GenerateWhatBuffer() noexcept
{
	m_whatBuffer = std::format("{}\n{}\n{}", GetType(), m_errorText, GetOriginString());
}
