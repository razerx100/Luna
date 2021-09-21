#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__
#include <cinttypes>
#include <memory>
#include <optional>
#include <string>

#ifdef BUILD_LUNA
#define LUNA_DLL __declspec(dllexport)
#else
#define LUNA_DLL __declspec(dllimport)
#endif

class LUNA_DLL Window {
public:
	virtual ~Window() = default;

	virtual void SetTitle(const char* title) = 0;
	virtual int Update() = 0;
	virtual void SetWindowIcon(const char* iconPath) = 0;
	virtual void EnableCursor() noexcept = 0;
	virtual void DisableCursor() noexcept = 0;
	virtual void ConfineCursor() noexcept = 0;
	virtual void FreeCursor() noexcept = 0;

	virtual bool IsCursorEnabled() const noexcept = 0;
	virtual void* GetWindowHandle() const noexcept = 0;
};

LUNA_DLL Window* _cdecl GetWindowInstance() noexcept;
LUNA_DLL void _cdecl InitWindowInstance(
	int width, int height, const char* name
) noexcept;

#endif
