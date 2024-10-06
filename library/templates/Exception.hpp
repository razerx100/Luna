#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_
#include <exception>
#include <string>

class Exception : public std::exception
{
public:
	Exception(std::int32_t line, std::string file);

	[[nodiscard]]
	const char* what() const noexcept override { return m_whatBuffer.c_str(); }

protected:
	[[nodiscard]]
	std::int32_t GetLine() const noexcept { return m_line; }
	[[nodiscard]]
	std::string GetFile() const noexcept { return m_file; }
	[[nodiscard]]
	std::string GetOriginString() const noexcept;

private:
	[[nodiscard]]
	const char* GetType() const noexcept { return "Exception"; }

	void GenerateWhatBuffer() noexcept;

private:
	std::int32_t m_line;
	std::string  m_file;

protected:
	std::string  m_whatBuffer;
};

class GenericException final : public Exception
{
public:
	GenericException(std::int32_t line, std::string file, std::string errorText);

private:
	[[nodiscard]]
	const char* GetType() const noexcept { return "GenericException"; }

	void GenerateWhatBuffer() noexcept;

private:
	std::string m_errorText;
};
#endif
