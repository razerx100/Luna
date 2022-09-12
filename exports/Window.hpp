#ifndef WINDOW_HPP_
#define WINDOW_HPP_
#include <string>
#include <optional>
#include <memory>

class Window {
public:
	virtual ~Window() = default;

	virtual void SetInputManager(std::shared_ptr<class InputManager> ioMan) = 0;
	virtual void SetTitle(const std::string& title) = 0;
	virtual void SetRenderer(std::shared_ptr<class Renderer> renderer) noexcept = 0;

	[[nodiscard]]
	virtual std::optional<int> Update() = 0;

	virtual void UpdateIndependentInputs() const noexcept = 0;
	virtual void SetWindowIcon(const std::wstring& iconPath) = 0;
	virtual void EnableCursor() noexcept = 0;
	virtual void DisableCursor() noexcept = 0;
	virtual void ConfineCursor() noexcept = 0;
	virtual void FreeCursor() noexcept = 0;

	[[nodiscard]]
	virtual bool IsCursorEnabled() const noexcept = 0;
	[[nodiscard]]
	virtual bool IsMinimized() const noexcept = 0;
	[[nodiscard]]
	virtual float GetAspectRatio() const noexcept = 0;
	[[nodiscard]]
	virtual void* GetWindowHandle() const noexcept = 0;
	[[nodiscard]]
	virtual void* GetModuleInstance() const noexcept = 0;
};

#endif
