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
	virtual void SetWindowIcon(const std::string& iconPath) = 0;
	virtual void EnableCursor() noexcept = 0;
	virtual void DisableCursor() noexcept = 0;
	virtual void ConfineCursor() noexcept = 0;
	virtual void FreeCursor() noexcept = 0;

	virtual bool IsCursorEnabled() const noexcept = 0;
	virtual void* GetWindowHandle() const noexcept = 0;

private:
	static std::unique_ptr<Window> s_instance;
};
#endif
