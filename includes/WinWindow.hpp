#ifndef WIN_WINDOW_HPP_
#define WIN_WINDOW_HPP_
#include <CleanWin.hpp>
#include <Window.hpp>
#include <vector>
#include <InputManager.hpp>
#include <IGraphicsEngine.hpp>

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
	WinWindow(std::uint32_t width, std::uint32_t height, const char* name);
	~WinWindow() noexcept override;

	WinWindow(const WinWindow&) = delete;
	WinWindow& operator=(const WinWindow&) = delete;

	[[nodiscard]]
	bool IsCursorEnabled() const noexcept override;
	[[nodiscard]]
	bool IsMinimized() const noexcept override;
	[[nodiscard]]
	float GetAspectRatio() const noexcept override;
	[[nodiscard]]
	void* GetWindowHandle() const noexcept override;
	[[nodiscard]]
	void* GetModuleInstance() const noexcept override;

	void SetInputManager(std::shared_ptr<InputManager> ioMan) override;

	void SetTitle(const std::string& title) override;
	void SetRenderer(std::shared_ptr<GraphicsEngine> renderer) noexcept override;

	void SetWindowIcon(const std::string& iconPath) override;
	void EnableCursor() noexcept override;
	void DisableCursor() noexcept override;
	void ConfineCursor() noexcept override;
	void FreeCursor() noexcept override;

	[[nodiscard]]
	std::optional<int> Update() override;

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
	std::shared_ptr<InputManager> m_pInputManager;
	std::shared_ptr<GraphicsEngine> m_pGraphicsEngine;

private:
	std::uint32_t m_width;
	std::uint32_t m_height;
	HWND m_hWnd;

	bool m_fullScreenMode;
	std::uint32_t m_windowStyle;
	RECT m_windowRect;
	WindowClass m_windowClass;

	bool m_cursorEnabled;
	bool m_isMinimized;
	std::vector<std::uint8_t> m_rawInputBuffer;
};

SKeyCodes GetSKeyCodes(std::uint16_t nativeKeycode) noexcept;
std::pair<std::uint8_t, std::uint8_t> ProcessMouseRawButtons(
	std::uint16_t newState
) noexcept;
std::uint16_t ProcessGamepadRawButtons(std::uint16_t state) noexcept;

#endif