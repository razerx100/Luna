#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__
#include <exception>
#include <string>

class Exception : public std::exception {
public:
	Exception(int line, const char* file) noexcept;

	virtual const char* GetType() const noexcept;
	// Call it in the end child's constructor
	virtual void GenerateWhatBuffer() noexcept;

	const char* what() const noexcept override;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;

private:
	int m_line;
	std::string m_file;

protected:
	std::string m_whatBuffer;
};

class GenericException : public Exception {
public:
	GenericException(
		int line, const char* file,
		const std::string& errorText
	) noexcept;

	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	void GenerateWhatBuffer() noexcept override;

private:
	std::string m_errorText;
};

// Must define this function in Window implementation
void ExceptionMessageBox(
	const std::string& exceptionDetails,
	const std::string& exceptionType
);

#endif
