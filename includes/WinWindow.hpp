#ifndef WIN_WINDOW_HPP_
#define WIN_WINDOW_HPP_
#include <CleanWin.hpp>
#include <Window.hpp>
#include <vector>
#include <InputManager.hpp>
#include <Renderer.hpp>

class WinWindow final : public Window {
private:

	class WindowClass {
	public:
		WindowClass() noexcept;
		~WindowClass() noexcept;

		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;

		const char* GetName() noexcept;
		void Register() noexcept;

		[[nodiscard]]
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
	void SetRenderer(std::shared_ptr<Renderer> renderer) noexcept override;

	void SetWindowIcon(const std::wstring& iconPath) override;
	void EnableCursor() noexcept override;
	void DisableCursor() noexcept override;
	void ConfineCursor() noexcept override;
	void FreeCursor() noexcept override;
	void UpdateIndependentInputs() const noexcept override;

	[[nodiscard]]
	std::optional<int> Update() override;

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgWrap(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	[[nodiscard]]
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	[[nodiscard]]
	HICON LoadIconFromPath(const wchar_t* iconPath);
	[[nodiscard]]
	bool IsKeyDown(int vKey) const noexcept;

	void ToggleFullScreenMode();
	void HideCursor() noexcept;
	void ShowCursor() noexcept;

private:
	std::shared_ptr<InputManager> m_pInputManager;
	std::shared_ptr<Renderer> m_pRenderer;

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

[[nodiscard]]
SKeyCodes GetSKeyCodes(std::uint16_t nativeKeycode) noexcept;
[[nodiscard]]
std::pair<std::uint8_t, std::uint8_t> ProcessMouseRawButtons(
	std::uint16_t newState
) noexcept;
#endif