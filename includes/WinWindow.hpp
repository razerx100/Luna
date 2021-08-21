#ifndef __WIN_WINDOW_HPP__
#define __WIN_WINDOW_HPP__
#include <CleanWin.hpp>
#include <Window.hpp>
#include <Keyboard.hpp>
#include <Mouse.hpp>
#include <optional>
#include <memory>

class WinWindow : public Window {
private:

	class WindowClass {
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "Direct3D Engine Window";
		static WindowClass s_wndClass;
		HINSTANCE m_hInst;
	};

public:
	WinWindow(int width, int height, const char* name);
	~WinWindow();
	WinWindow(const WinWindow&) = delete;

	WinWindow& operator=(const WinWindow&) = delete;

	void SetTitle(const std::string& title) override;

	bool IsCursorEnabled() const noexcept;
	void* GetWindowHandle() const noexcept override;

	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;

	std::optional<int> Update() override;

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgWrap(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	void ToggleFullScreenMode();
	void HideCursor() noexcept;
	void ShowCursor() noexcept;

public:
	Keyboard* m_kbRef;
	Mouse* m_mouseRef;

private:
	int m_width;
	int m_height;
	HWND m_hWnd;

	bool m_fullScreenMode;
	std::uint32_t m_windowStyle;
	RECT m_windowRect;

	bool m_cursorEnabled;
	std::vector<std::uint8_t> m_rawInputBuffer;
};

#endif