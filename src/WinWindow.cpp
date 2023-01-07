#include <WinWindow.hpp>
#include <WindowThrowMacros.hpp>
#include <filesystem>
#include <hidusage.h>
#include <XBoxController.hpp>
#include <Xinput.h>

WinWindow::WindowClass::WindowClass() noexcept : m_wndClass{} {
	m_wndClass.cbSize = static_cast<UINT>(sizeof(m_wndClass));
	m_wndClass.style = CS_OWNDC;
	m_wndClass.lpfnWndProc = HandleMsgSetup;
	m_wndClass.cbClsExtra = 0;
	m_wndClass.cbWndExtra = 0;
	m_wndClass.hInstance = nullptr;
	m_wndClass.hIcon = nullptr;
	m_wndClass.hCursor = nullptr;
	m_wndClass.hbrBackground = nullptr;
	m_wndClass.lpszMenuName = nullptr;
	m_wndClass.lpszClassName = GetName();
	m_wndClass.hIconSm = nullptr;
}

WinWindow::WindowClass::~WindowClass() noexcept {
	UnregisterClassA(GetName(), m_wndClass.hInstance);
}

const char* WinWindow::WindowClass::GetName() noexcept {
	return wndClassName;
}

void WinWindow::WindowClass::Register() noexcept {
	m_wndClass.hInstance = GetModuleHandleA(nullptr);

	RegisterClassEx(&m_wndClass);
}

HINSTANCE WinWindow::WindowClass::GetHInstance() const noexcept {
	return m_wndClass.hInstance;
}

// Window
WinWindow::WinWindow(
	std::uint32_t width, std::uint32_t height, const char* name
) : m_width(width), m_height(height), m_hWnd(nullptr),
	m_fullScreenMode(false),
	m_windowStyle(WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU),
	m_windowRect{},
	m_cursorEnabled(true), m_isMinimized(false) {

	m_windowClass.Register();

	RECT wr{
		.left = 0,
		.top = 0,
		.right = static_cast<LONG>(width),
		.bottom = static_cast<LONG>(height)
	};

	if (!AdjustWindowRect(&wr, m_windowStyle, FALSE))
		throw WIN32_LAST_EXCEPT();

	m_hWnd = CreateWindowExA(
		0,
		m_windowClass.GetName(), name,
		m_windowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		static_cast<int>(wr.right - wr.left),
		static_cast<int>(wr.bottom - wr.top),
		nullptr, nullptr, m_windowClass.GetHInstance(), this
	);

	if (!m_hWnd)
		throw WIN32_LAST_EXCEPT();

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
}

WinWindow::~WinWindow() noexcept {
	SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(DefWindowProcA));
}

LRESULT CALLBACK WinWindow::HandleMsgSetup(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
) noexcept {
	if (msg == WM_NCCREATE) {
		auto pCreate = reinterpret_cast<CREATESTRUCTA*>(lParam);
		auto pWnd = static_cast<WinWindow*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(
			hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WinWindow::HandleMsgWrap)
		);

		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProcA(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WinWindow::HandleMsgWrap(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
) noexcept {
	auto pWnd = reinterpret_cast<WinWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT WinWindow::HandleMsg(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
) noexcept {
	switch (msg) {
	case WM_CLOSE: {
		DestroyWindow(m_hWnd);
		return 0;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}
	// Clear keystate when window loses focus to prevent input getting stuck
	case WM_KILLFOCUS: {
		if (m_pInputManager)
			m_pInputManager->ClearInputStates();

		break;
	}
	case WM_SIZE: {
		RECT clientRect = {};
		GetClientRect(m_hWnd, &clientRect);

		if (wParam != SIZE_MINIMIZED) {
			m_isMinimized = false;
			m_width = static_cast<std::uint32_t>(clientRect.right - clientRect.left);
			m_height = static_cast<std::uint32_t>(clientRect.bottom - clientRect.top);

			if (m_pRenderer)
				m_pRenderer->Resize(m_width, m_height);
		}
		else
			m_isMinimized = true;

		if(!m_cursorEnabled)
			ConfineCursor();

		break;
	}
	case WM_ACTIVATE: {
		if (!m_cursorEnabled)
			if (wParam & WA_ACTIVE || wParam & WA_CLICKACTIVE)
				ConfineCursor();
			else
				FreeCursor();

		break;
	}
	case WM_INPUT_DEVICE_CHANGE: {
		if (wParam == GIDC_REMOVAL && m_pInputManager) {
			m_pInputManager->DisconnectDevice(static_cast<std::uint64_t>(lParam));

			DisconnectXBoxController(m_pInputManager.get());
		}

		break;
	}
	/************* KEYBOARD MESSAGES *************/
	case WM_SYSKEYDOWN: {
		if ((wParam == VK_RETURN) && (lParam & static_cast<LONG_PTR>(1u << 29u)))
			ToggleFullScreenMode();

		break;
	}
	case WM_CHAR: {
#ifdef _IMGUI
		if (imIO.WantCaptureKeyboard)
			break;
#endif

		if (m_pInputManager)
			m_pInputManager->GetKeyboard()->OnChar(static_cast<char>(wParam));

		break;
	}
	/************* END KEYBOARD MESSAGES *************/
	/************* RAW MOUSE MESSAGES *************/
	case WM_INPUT: {
		UINT size = 0;

		// Get raw data size with nullptr as buffer
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size,
			static_cast<UINT>(sizeof(RAWINPUTHEADER))
		) == -1)
			break;

		m_rawInputBuffer.resize(size);

		// Get raw data by passing buffer
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
			std::data(m_rawInputBuffer), &size,
			static_cast<UINT>(sizeof(RAWINPUTHEADER))
		) != size)
			break;

		auto rawInput = reinterpret_cast<const RAWINPUT*>(std::data(m_rawInputBuffer));
		const RAWINPUTHEADER& rawHeader = rawInput->header;

		if (rawHeader.dwType == RIM_TYPEMOUSE) {
			IMouse* pMouseRef = m_pInputManager->GetMouseByHandle(
				reinterpret_cast<std::uint64_t>(rawHeader.hDevice)
			);

			const RAWMOUSE& rawMouse = rawInput->data.mouse;

			if (rawMouse.usButtonFlags & RI_MOUSE_WHEEL)
				pMouseRef->OnWheelDelta(static_cast<short>(rawMouse.usButtonData));
			else if (rawMouse.usButtonFlags) {
				auto [pressedButtons, releasedButtons] = ProcessMouseRawButtons(
					rawMouse.usButtonFlags
				);
				pMouseRef->SetPressState(pressedButtons);
				pMouseRef->SetReleaseState(releasedButtons);
			}

			if (rawMouse.lLastX != 0 || rawMouse.lLastY != 0)
				pMouseRef->OnMouseMove(rawMouse.lLastX, rawMouse.lLastY);
		}
		else if (rawHeader.dwType == RIM_TYPEKEYBOARD) {
			IKeyboard* pKeyboardRef = m_pInputManager->GetKeyboardByHandle(
				reinterpret_cast<std::uint64_t>(rawHeader.hDevice)
			);

			const RAWKEYBOARD& rawKeyboard = rawInput->data.keyboard;

			UINT legacyMessage = rawKeyboard.Message;
			if (legacyMessage == WM_KEYDOWN || legacyMessage == WM_SYSKEYDOWN) {
				switch (rawKeyboard.VKey) {
				case VK_SHIFT: {
					if (IsKeyDown(VK_LSHIFT))
						pKeyboardRef->OnKeyPressed(GetSKeyCodes(VK_LSHIFT));
					else if (IsKeyDown(VK_RSHIFT))
						pKeyboardRef->OnKeyPressed(GetSKeyCodes(VK_RSHIFT));

					break;
				}
				case VK_MENU: {
					if (IsKeyDown(VK_LMENU))
						pKeyboardRef->OnKeyPressed(GetSKeyCodes(VK_LMENU));
					else if (IsKeyDown(VK_RMENU))
						pKeyboardRef->OnKeyPressed(GetSKeyCodes(VK_RMENU));

					break;
				}
				case VK_CONTROL: {
					if (IsKeyDown(VK_LCONTROL))
						pKeyboardRef->OnKeyPressed(GetSKeyCodes(VK_LCONTROL));
					else if (IsKeyDown(VK_RCONTROL))
						pKeyboardRef->OnKeyPressed(GetSKeyCodes(VK_RCONTROL));

					break;
				}
				}

				pKeyboardRef->OnKeyPressed(GetSKeyCodes(rawKeyboard.VKey));
			}
			else if (legacyMessage == WM_KEYUP || legacyMessage == WM_SYSKEYUP) {
				switch (rawKeyboard.VKey) {
				case VK_SHIFT: {
					if (!IsKeyDown(VK_LSHIFT))
						pKeyboardRef->OnKeyReleased(GetSKeyCodes(VK_LSHIFT));
					else if (!IsKeyDown(VK_RSHIFT))
						pKeyboardRef->OnKeyReleased(GetSKeyCodes(VK_RSHIFT));

					break;
				}
				case VK_MENU: {
					if (!IsKeyDown(VK_LMENU))
						pKeyboardRef->OnKeyReleased(GetSKeyCodes(VK_LMENU));
					else if (!IsKeyDown(VK_RMENU))
						pKeyboardRef->OnKeyReleased(GetSKeyCodes(VK_RMENU));

					break;
				}
				case VK_CONTROL: {
					if (!IsKeyDown(VK_LCONTROL))
						pKeyboardRef->OnKeyReleased(GetSKeyCodes(VK_LCONTROL));
					else if (!IsKeyDown(VK_RCONTROL))
						pKeyboardRef->OnKeyReleased(GetSKeyCodes(VK_RCONTROL));

					break;
				}
				}

				pKeyboardRef->OnKeyReleased(GetSKeyCodes(rawKeyboard.VKey));
			}
		}

		break;
	}
	/************* END RAW MOUSE MESSAGES *************/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void WinWindow::SetTitle(const std::string& title) {
	if (!SetWindowTextA(m_hWnd, title.c_str()))
		throw WIN32_LAST_EXCEPT();
}

std::optional<int> WinWindow::Update() {
	MSG msg{};

	while (PeekMessageA(&msg, nullptr , 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
			return static_cast<int>(msg.wParam);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

void WinWindow::ToggleFullScreenMode() {
	if (m_fullScreenMode) {
		SetWindowLong(m_hWnd, GWL_STYLE, m_windowStyle);

		SetWindowPos(
			m_hWnd,
			HWND_NOTOPMOST,
			m_windowRect.left,
			m_windowRect.top,
			m_windowRect.right - m_windowRect.left,
			m_windowRect.bottom - m_windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE
		);

		ShowWindow(m_hWnd, SW_NORMAL);
	}
	else {
		GetWindowRect(m_hWnd, &m_windowRect);

		SetWindowLong(
			m_hWnd, GWL_STYLE,
			m_windowStyle & ~(
				WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX |
				WS_SYSMENU)
		);

		// Needed for multi monitor setups
		if (m_pRenderer) {
			auto [width, height] = m_pRenderer->GetFirstDisplayCoordinates();

			RECT renderingMonitorCoordinate{
				.right = static_cast<LONG>(width),
				.bottom = static_cast<LONG>(height)
			};

			SetWindowPos(
				m_hWnd,
				HWND_TOPMOST,
				renderingMonitorCoordinate.left,
				renderingMonitorCoordinate.top,
				renderingMonitorCoordinate.right,
				renderingMonitorCoordinate.bottom,
				SWP_FRAMECHANGED | SWP_NOACTIVATE
			);
		}

		ShowWindow(m_hWnd, SW_MAXIMIZE);
	}

	m_fullScreenMode = !m_fullScreenMode;
}

void WinWindow::EnableCursor() noexcept {
	m_cursorEnabled = true;

	ShowCursor();
	FreeCursor();
}

void WinWindow::DisableCursor() noexcept {
	m_cursorEnabled = false;

	HideCursor();
	ConfineCursor();
}

void WinWindow::HideCursor() noexcept {
	while (::ShowCursor(FALSE) >= 0);
}

void WinWindow::ShowCursor() noexcept {
	while (::ShowCursor(TRUE) < 0);
}

void WinWindow::ConfineCursor() noexcept {
	RECT rect{};
	GetClientRect(m_hWnd, &rect);
	MapWindowPoints(m_hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}

void WinWindow::FreeCursor() noexcept {
	ClipCursor(nullptr);
}

bool WinWindow::IsCursorEnabled() const noexcept {
	return m_cursorEnabled;
}

void* WinWindow::GetWindowHandle() const noexcept {
	return reinterpret_cast<void*>(m_hWnd);
}

void* WinWindow::GetModuleInstance() const noexcept {
	return reinterpret_cast<void*>(m_windowClass.GetHInstance());
}

HICON WinWindow::LoadIconFromPath(const wchar_t* iconPath) {
	std::wstring relativePath = std::filesystem::current_path().wstring();

	return reinterpret_cast<HICON>(
		LoadImageW(
			nullptr, (relativePath + L"\\" + iconPath).c_str(), IMAGE_ICON, 0, 0,
			LR_DEFAULTSIZE | LR_LOADFROMFILE
		));
}

void WinWindow::SetWindowIcon(const std::wstring& iconPath) {
	HICON hIcon = LoadIconFromPath(iconPath.c_str());

	SendMessageA(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	SendMessageA(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
}

bool WinWindow::IsMinimized() const noexcept {
	return m_isMinimized;
}

void WinWindow::SetRenderer(std::shared_ptr<Renderer> renderer) noexcept {
	m_pRenderer = std::move(renderer);
}

void WinWindow::SetInputManager(std::shared_ptr<InputManager> ioMan) {
	m_pInputManager = std::move(ioMan);

	std::vector<RAWINPUTDEVICE> rawInputIDs;
	size_t keyboardCount = m_pInputManager->GetKeyboardCount();
	size_t mouseCount = m_pInputManager->GetMouseCount();
	size_t gamepadCount = m_pInputManager->GetGamepadCount();

	if (gamepadCount >= 5u)
		WIN32_GENERIC_THROW(
			"Maximum supported XBox gamepads are four."
		);

	for (size_t index = 0u; index < keyboardCount; ++index)
		rawInputIDs.emplace_back(
			RAWINPUTDEVICE{
				HID_USAGE_PAGE_GENERIC,
				HID_USAGE_GENERIC_KEYBOARD,
				RIDEV_DEVNOTIFY,
				m_hWnd
			}
		);

	for(size_t index = 0u; index < mouseCount; ++index)
		rawInputIDs.emplace_back(
			RAWINPUTDEVICE{
				HID_USAGE_PAGE_GENERIC,
				HID_USAGE_GENERIC_MOUSE,
				RIDEV_DEVNOTIFY,
				m_hWnd
			}
		);

	std::vector<IGamepad*> pGamepadRefs = m_pInputManager->GetGamepadRefs();

	for (IGamepad* gamepad : pGamepadRefs) {
		if(!gamepad->GetLeftThumbStickDeadZone())
			gamepad->SetLeftThumbStickDeadZone(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

		if(!gamepad->GetRightThumbStickDeadZone())
			gamepad->SetRightThumbStickDeadZone(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

		if (!gamepad->GetTriggerThreshold())
			gamepad->SetTriggerThreshold(XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

		rawInputIDs.emplace_back(
			RAWINPUTDEVICE{
				HID_USAGE_PAGE_GENERIC,
				HID_USAGE_GENERIC_GAMEPAD,
				RIDEV_DEVNOTIFY,
				m_hWnd
			}
		);
	}

	if (!RegisterRawInputDevices(
		std::data(rawInputIDs),
		static_cast<UINT>(std::size(rawInputIDs)), static_cast<UINT>(sizeof(RAWINPUTDEVICE))
	))
		throw WIN32_LAST_EXCEPT();
}

float WinWindow::GetAspectRatio() const noexcept {
	return static_cast<float>(m_width) / m_height;
}

void WinWindow::UpdateIndependentInputs() const noexcept {
	CheckXBoxControllerStates(m_pInputManager.get());
}

bool WinWindow::IsKeyDown(int vKey) const noexcept {
	union {
		SHORT _signed;
		unsigned short _unsigned;
	}keyState{ GetKeyState(vKey) };

	return keyState._unsigned & 0x8000u;
}
