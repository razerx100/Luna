#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__
#include <cstdint>
#include <string>

#ifdef BUILD_LUNA
#define LUNA_DLL __declspec(dllexport)
#else
#define LUNA_DLL __declspec(dllimport)
#endif

class LUNA_DLL Window {
public:
	virtual ~Window() = default;

	virtual void SetTitle(const std::string& title) = 0;
	virtual void SetGraphicsEngineRef(class GraphicsEngine* gfxEngine) noexcept = 0;

	[[nodiscard]]
	virtual int Update() = 0;
	virtual void SetWindowIcon(const std::string& iconPath) = 0;
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
