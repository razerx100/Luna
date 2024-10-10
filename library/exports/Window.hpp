#ifndef WINDOW_HPP_
#define WINDOW_HPP_
#include <string>
#include <memory>
#include <cstdint>

class Window
{
public:
	virtual ~Window() = default;

	virtual void SetTitle(const std::string& title) = 0;

	[[nodiscard]]
	// 0 means exited without any errors. 1 means the window is still active.
	virtual std::int32_t Update() = 0;

	virtual void ToggleFullscreen(std::uint32_t width, std::uint32_t height) noexcept = 0;
	virtual void SetWindowIcon(const std::wstring& iconPath) = 0;

	virtual void EnableCursor() noexcept = 0;
	virtual void DisableCursor() noexcept = 0;
	virtual void ConfineCursor() noexcept = 0;
	virtual void FreeCursor() noexcept = 0;

	[[nodiscard]]
	virtual bool IsCursorEnabled() const noexcept = 0;
	[[nodiscard]]
	virtual bool IsMinimised() const noexcept = 0;
	[[nodiscard]]
	virtual float GetAspectRatio() const noexcept = 0;
	[[nodiscard]]
	virtual void* GetWindowHandle() const noexcept = 0;

	// Windows functions
	[[nodiscard]]
	virtual void* GetModuleInstance() const noexcept = 0;

	virtual void AddInputCallback(
		void(*callback)(void*, std::uint32_t, std::uint64_t, std::uint64_t, void*),
		void* extraData = nullptr
	) noexcept = 0;
};
#endif
