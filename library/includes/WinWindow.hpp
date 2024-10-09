#ifndef WIN_WINDOW_HPP_
#define WIN_WINDOW_HPP_
#include <CleanWin.hpp>
#include <Window.hpp>
#include <vector>
#include <InputManager.hpp>
#include <Renderer.hpp>

class WinWindow final : public Window
{
	class WindowClass
	{
	public:
		WindowClass();
		~WindowClass() noexcept;

		[[nodiscard]]
		static const char* GetName() noexcept { return s_wndClassName; }

		void Register() noexcept;

		[[nodiscard]]
		HINSTANCE GetHInstance() const noexcept { return m_wndClass.hInstance; }

	private:
		WNDCLASSEX m_wndClass;

		static constexpr const char* s_wndClassName = "Luna";

	public:
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;

		WindowClass(WindowClass&& other) noexcept
			: m_wndClass{ other.m_wndClass }
		{
			other.m_wndClass.hInstance = nullptr;
		}
		WindowClass& operator=(WindowClass&& other) noexcept
		{
			m_wndClass                 = other.m_wndClass;
			other.m_wndClass.hInstance = nullptr;

			return *this;
		}
	};

	struct CallbackData
	{
		void(*callback)(void*, std::uint32_t, std::uint64_t, std::uint64_t, void*);
		void* extraData;
	};

public:
	WinWindow(std::uint32_t width, std::uint32_t height, const char* name);
	~WinWindow() noexcept override;

	[[nodiscard]]
	bool IsCursorEnabled() const noexcept override;
	[[nodiscard]]
	bool IsMinimised() const noexcept override;
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

	void AddInputCallback(
		void(*callback)(void*, std::uint32_t, std::uint64_t, std::uint64_t, void*),
		void* extraData = nullptr
	) noexcept override;

	[[nodiscard]]
	std::optional<int> Update() override;

private:
	static LRESULT CALLBACK HandleMsgSetup(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
	) noexcept;
	static LRESULT CALLBACK HandleMsgWrap(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
	) noexcept;

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
	std::shared_ptr<Renderer>     m_pRenderer;

	std::uint32_t             m_width;
	std::uint32_t             m_height;
	std::vector<CallbackData> m_inputCallbacks;
	HWND                      m_hWnd;
	RECT                      m_windowRect;
	WindowClass               m_windowClass;
	std::vector<std::uint8_t> m_rawInputBuffer;
	DWORD                     m_windowStyle;
	bool                      m_fullScreenMode;
	bool                      m_cursorEnabled;
	bool                      m_isMinimised;
	bool                      m_multimonitor;

public:
	WinWindow(const WinWindow&) = delete;
	WinWindow& operator=(const WinWindow&) = delete;

	WinWindow(WinWindow&& other) noexcept
		: m_pInputManager{ std::move(other.m_pInputManager) },
		m_pRenderer{ std::move(other.m_pRenderer) },
		m_width{ other.m_width },
		m_height{ other.m_height },
		m_inputCallbacks{ std::move(other.m_inputCallbacks) },
		m_hWnd{ std::exchange(other.m_hWnd, nullptr) },
		m_windowRect{ other.m_windowRect },
		m_windowClass{ std::move(other.m_windowClass) },
		m_rawInputBuffer{ std::move(other.m_rawInputBuffer) },
		m_windowStyle{ other.m_windowStyle },
		m_fullScreenMode{ other.m_fullScreenMode },
		m_cursorEnabled{ other.m_cursorEnabled },
		m_isMinimised{ other.m_isMinimised },
		m_multimonitor{ other.m_multimonitor }
	{}
	WinWindow& operator=(WinWindow&& other) noexcept
	{
		m_pInputManager  = std::move(other.m_pInputManager);
		m_pRenderer      = std::move(other.m_pRenderer);
		m_width          = other.m_width;
		m_height         = other.m_height;
		m_inputCallbacks = std::move(other.m_inputCallbacks);
		m_hWnd           = std::exchange(other.m_hWnd, nullptr);
		m_windowRect     = other.m_windowRect;
		m_windowClass    = std::move(other.m_windowClass);
		m_rawInputBuffer = std::move(other.m_rawInputBuffer);
		m_windowStyle    = other.m_windowStyle;
		m_fullScreenMode = other.m_fullScreenMode;
		m_cursorEnabled  = other.m_cursorEnabled;
		m_isMinimised    = other.m_isMinimised;
		m_multimonitor   = other.m_multimonitor;


		return *this;
	}
};

[[nodiscard]]
SKeyCodes GetSKeyCodes(std::uint16_t nativeKeycode) noexcept;
[[nodiscard]]
std::pair<std::uint8_t, std::uint8_t> ProcessMouseRawButtons(
	std::uint16_t newState
) noexcept;
#endif