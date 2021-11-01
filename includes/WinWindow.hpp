#ifndef __WIN_WINDOW_HPP__
#define __WIN_WINDOW_HPP__
#include <CleanWin.hpp>
#include <Window.hpp>
#include <vector>
#include <InputManager.hpp>

class WinWindow : public Window {
private:

	class WindowClass {
	public:
		WindowClass() noexcept;
		~WindowClass() noexcept;

		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;

		const char* GetName() noexcept;
		void Register() noexcept;
		HINSTANCE GetHInstance() const noexcept;

	private:
		static constexpr const char* wndClassName = "Luna";
		WNDCLASSEX m_wndClass;
	};

public:
	WinWindow(int width, int height, const char* name);
	~WinWindow() noexcept;

	WinWindow(const WinWindow&) = delete;
	WinWindow& operator=(const WinWindow&) = delete;

	bool IsCursorEnabled() const noexcept override;
	void* GetWindowHandle() const noexcept override;

	void SetTitle(const char* title) override;
	void SetWindowIcon(const char* iconPath) override;
	void EnableCursor() noexcept override;
	void DisableCursor() noexcept override;
	void ConfineCursor() noexcept override;
	void FreeCursor() noexcept override;

	int Update() override;

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgWrap(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	void ToggleFullScreenMode();
	void HideCursor() noexcept;
	void ShowCursor() noexcept;
	HICON LoadIconFromPath(const char* iconPath);

	float GetMagnitude(std::int16_t x, std::int16_t y) const noexcept;
	float ProcessDeadZone(
		float magnitude, std::uint32_t maxValue, std::uint32_t deadZone
	) const noexcept;
	ASData ProcessASMagnitude(
		float magnitude, std::int16_t x, std::int16_t y, std::uint32_t deadZone
	) const noexcept;

private:
	InputManager* m_pInputManagerRef;

private:
	int m_width;
	int m_height;
	HWND m_hWnd;

	bool m_fullScreenMode;
	std::uint32_t m_windowStyle;
	RECT m_windowRect;
	WindowClass m_windowClass;

	bool m_cursorEnabled;
	std::vector<std::uint8_t> m_rawInputBuffer;
};

SKeyCodes GetSKeyCodes(std::uint16_t nativeKeycode);

#endif