#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_
#include <exception>
#include <string>

class Exception : public std::exception {
public:
	Exception(int line, const char* file) noexcept;

	[[nodiscard]]
	virtual const char* GetType() const noexcept;
	// Call it in the end child's constructor
	virtual void GenerateWhatBuffer() noexcept;

	[[nodiscard]]
	const char* what() const noexcept override;
	[[nodiscard]]
	int GetLine() const noexcept;
	[[nodiscard]]
	const std::string& GetFile() const noexcept;
	[[nodiscard]]
	std::string GetOriginString() const noexcept;

private:
	int m_line;
	std::string m_file;

protected:
	std::string m_whatBuffer;
};

class GenericException final : public Exception {
public:
	GenericException(
		int line, const char* file,
		const std::string& errorText
	) noexcept;

	[[nodiscard]]
	const char* what() const noexcept override;
	[[nodiscard]]
	const char* GetType() const noexcept override;
	void GenerateWhatBuffer() noexcept override;

private:
	std::string m_errorText;
};
#endif
