#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__
#include <cinttypes>
#include <memory>
#include <optional>
#include <string>

class Window {
public:
	virtual ~Window() = default;

	static void Init(std::unique_ptr<Window> instance);
	static Window* GetRef() noexcept;

	virtual void SetTitle(const std::string& title) = 0;
	virtual std::optional<int> Update() = 0;
	virtual void* GetWindowHandle() const noexcept = 0;
	virtual void SetIcon(std::uint16_t iconHandle) = 0;

private:
	static std::unique_ptr<Window> s_instance;
};
#endif
