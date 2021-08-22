#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__
#include <exception>
#include <string>

class Exception : public std::exception {
public:
	Exception(int line, const char* file) noexcept;

	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;

private:
	int m_line;
	std::string m_file;

protected:
	mutable std::string m_whatBuffer;
};

class GenericException : public std::exception {
public:
	GenericException(const std::string& errorText) noexcept;

	const char* what() const noexcept override;

private:
	std::string m_errorText;
};

#endif
