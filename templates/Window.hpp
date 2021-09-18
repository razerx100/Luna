#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__
#include <cinttypes>
#include <memory>
#include <optional>
#include <string>
#include <GenericSingleton.hpp>

class Window : public GenericSingleton<Window> {
public:
	virtual ~Window() = default;

	virtual void SetTitle(const std::string& title) = 0;
	virtual std::optional<int> Update() = 0;
	virtual void SetWindowIcon(const std::string& iconPath) = 0;
	virtual void EnableCursor() noexcept = 0;
	virtual void DisableCursor() noexcept = 0;
	virtual void ConfineCursor() noexcept = 0;
	virtual void FreeCursor() noexcept = 0;

	virtual bool IsCursorEnabled() const noexcept = 0;
	virtual void* GetWindowHandle() const noexcept = 0;
};
#endif
