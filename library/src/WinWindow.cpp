#include <WinWindow.hpp>
#include <WindowThrowMacros.hpp>
#include <cmath>
#include <filesystem>
#include <hidusage.h>
#include <XBoxController.hpp>
#include <Xinput.h>

WinWindow::WindowClass::WindowClass()
	: m_wndClass{
		.cbSize        = static_cast<UINT>(sizeof(m_wndClass)),
		.style         = CS_OWNDC,
		.lpfnWndProc   = HandleMsgSetup,
		.hInstance     = nullptr,
		.lpszClassName = GetName()
	}
{}

WinWindow::WindowClass::~WindowClass() noexcept
{
	if (m_wndClass.hInstance != nullptr)
		UnregisterClassA(GetName(), m_wndClass.hInstance);
}

void WinWindow::WindowClass::Register() noexcept
{
	m_wndClass.hInstance = GetModuleHandleA(nullptr);

	RegisterClassEx(&m_wndClass);
}

// Window
WinWindow::WinWindow(
	std::uint32_t width, std::uint32_t height, const char* name
) : m_pInputManager{}, m_pRenderer{},
	m_width{ width }, m_height{ height }, m_inputCallbacks{}, m_hWnd{ nullptr },
	m_windowRect{ 0l, 0l, 0l, 0l }, m_windowClass{},
	m_rawInputBuffer{},
	m_windowStyle{ WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU },
	m_fullScreenMode{ false }, m_cursorEnabled{ true }, m_isMinimised{ false },
	m_multimonitor{ false }
{
	m_windowClass.Register();

	RECT wr{
		.left   = 0l,
		.top    = 0l,
		.right  = static_cast<LONG>(width),
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

WinWindow::~WinWindow() noexcept
{
	SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(DefWindowProcA));
}

LRESULT CALLBACK WinWindow::HandleMsgSetup(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
) noexcept {
	if (msg == WM_NCCREATE)
	{
		auto pCreate = reinterpret_cast<CREATESTRUCTA*>(lParam);
		auto pWnd    = static_cast<WinWindow*>(pCreate->lpCreateParams);

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
	for (const CallbackData& callbackData : m_inputCallbacks)
		callbackData.callback(hWnd, msg, wParam, lParam, callbackData.extraData);

	switch (msg)
	{
	case WM_CLOSE:
	{
		DestroyWindow(m_hWnd);
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	// Clear keystate when window loses focus to prevent input getting stuck
	case WM_KILLFOCUS:
	{
		if (m_pInputManager)
			m_pInputManager->ClearInputStates();

		break;
	}
	case WM_SIZE:
	{
		RECT clientRect = {};
		GetClientRect(m_hWnd, &clientRect);

		if (wParam != SIZE_MINIMIZED)
		{
			m_isMinimised = false;
			m_width       = static_cast<std::uint32_t>(clientRect.right - clientRect.left);
			m_height      = static_cast<std::uint32_t>(clientRect.bottom - clientRect.top);

			if (m_pRenderer)
				m_pRenderer->Resize(m_width, m_height);
		}
		else
			m_isMinimised = true;

		if(!m_cursorEnabled)
			ConfineCursor();

		break;
	}
	case WM_ACTIVATE:
	{
		if (!m_cursorEnabled)
			if (wParam & WA_ACTIVE || wParam & WA_CLICKACTIVE)
				ConfineCursor();
			else
				FreeCursor();

		break;
	}
	case WM_INPUT_DEVICE_CHANGE:
	{
		if (wParam == GIDC_REMOVAL && m_pInputManager)
			DisconnectXBoxController(m_pInputManager.get());

		break;
	}
	/************* KEYBOARD MESSAGES *************/
	case WM_SYSKEYDOWN:
	{
		if ((wParam == VK_RETURN) && (lParam & 0x20000000ul)) // 29th bit checks if Alt is down
			ToggleFullScreenMode();

		break;
	}
	case WM_CHAR:
	{
		if (m_pInputManager)
			m_pInputManager->GetKeyboard().OnChar(static_cast<char>(wParam));

		break;
	}
	/************* END KEYBOARD MESSAGES *************/
	case WM_MOUSEMOVE:
	{
		if (!m_multimonitor)
		{
			std::uint16_t xCoord = LOWORD(lParam);
			std::uint16_t yCoord = HIWORD(lParam);

			Mouse& mouse = m_pInputManager->GetMouse();
			mouse.SetCurrentCursorCoord(xCoord, yCoord);
		}

		break;
	}
	/************* RAW MESSAGES *************/
	case WM_INPUT:
	{
		constexpr auto rawInputHeaderSize = static_cast<UINT>(sizeof(RAWINPUTHEADER));

		UINT bufferSize = 0u;

		GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &bufferSize,
			rawInputHeaderSize
		);

		if (bufferSize > std::size(m_rawInputBuffer))
			m_rawInputBuffer.resize(bufferSize);

		// Get raw data by passing buffer
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, std::data(m_rawInputBuffer),
			&bufferSize, rawInputHeaderSize
		) != bufferSize)
			break;

		auto rawInput = reinterpret_cast<const RAWINPUT*>(std::data(m_rawInputBuffer));

		const RAWINPUTHEADER& rawHeader = rawInput->header;

		if (rawHeader.dwType == RIM_TYPEMOUSE)
		{
			Mouse& mouse = m_pInputManager->GetMouse();

			const RAWMOUSE& rawMouse = rawInput->data.mouse;

			if (rawMouse.usButtonFlags)
			{
				if (rawMouse.usButtonFlags & RI_MOUSE_WHEEL)
					mouse.OnWheelDelta(static_cast<short>(rawMouse.usButtonData));

				auto [pressedButtons, releasedButtons] = ProcessMouseRawButtons(
					rawMouse.usButtonFlags
				);

				mouse.SetPressState(pressedButtons);
				mouse.SetReleaseState(releasedButtons);
			}

			if (m_multimonitor)
				if (rawMouse.lLastX != 0 || rawMouse.lLastY != 0)
				{
					constexpr USHORT multipleMonitor =
						MOUSE_MOVE_ABSOLUTE || MOUSE_VIRTUAL_DESKTOP;

					if (rawMouse.usButtonFlags & multipleMonitor)
					{
						int vDisplayHeight = GetSystemMetrics(SM_CXVIRTUALSCREEN);
						int vDisplayWidth  = GetSystemMetrics(SM_CYVIRTUALSCREEN);

						static constexpr float absoluteLimit = 65535.f;

						float absoluteX = std::round(
							(rawMouse.lLastX / absoluteLimit) * vDisplayWidth
						);
						float absoluteY = std::round(
							(rawMouse.lLastY / absoluteLimit) * vDisplayHeight
						);

						RECT windowRect{};
						GetWindowRect(m_hWnd, &windowRect);

						mouse.SetCurrentCursorCoord(
							static_cast<std::uint16_t>(absoluteX - windowRect.left),
							static_cast<std::uint16_t>(absoluteY - windowRect.top)
						);
					}
				}
		}
		else if (rawHeader.dwType == RIM_TYPEKEYBOARD)
		{
			Keyboard& keyboard = m_pInputManager->GetKeyboard();

			const RAWKEYBOARD& rawKeyboard = rawInput->data.keyboard;

			UINT legacyMessage = rawKeyboard.Message;

			if (legacyMessage == WM_KEYDOWN || legacyMessage == WM_SYSKEYDOWN)
			{
				switch (rawKeyboard.VKey)
				{
				case VK_SHIFT:
				{
					if (IsKeyDown(VK_LSHIFT))
						keyboard.OnKeyPressed(GetSKeyCodes(VK_LSHIFT));
					else if (IsKeyDown(VK_RSHIFT))
						keyboard.OnKeyPressed(GetSKeyCodes(VK_RSHIFT));

					break;
				}
				case VK_MENU:
				{
					if (IsKeyDown(VK_LMENU))
						keyboard.OnKeyPressed(GetSKeyCodes(VK_LMENU));
					else if (IsKeyDown(VK_RMENU))
						keyboard.OnKeyPressed(GetSKeyCodes(VK_RMENU));

					break;
				}
				case VK_CONTROL:
				{
					if (IsKeyDown(VK_LCONTROL))
						keyboard.OnKeyPressed(GetSKeyCodes(VK_LCONTROL));
					else if (IsKeyDown(VK_RCONTROL))
						keyboard.OnKeyPressed(GetSKeyCodes(VK_RCONTROL));

					break;
				}
				}

				keyboard.OnKeyPressed(GetSKeyCodes(rawKeyboard.VKey));
			}
			else if (legacyMessage == WM_KEYUP || legacyMessage == WM_SYSKEYUP)
			{
				switch (rawKeyboard.VKey)
				{
				case VK_SHIFT:
				{
					if (!IsKeyDown(VK_LSHIFT))
						keyboard.OnKeyReleased(GetSKeyCodes(VK_LSHIFT));
					else if (!IsKeyDown(VK_RSHIFT))
						keyboard.OnKeyReleased(GetSKeyCodes(VK_RSHIFT));

					break;
				}
				case VK_MENU:
				{
					if (!IsKeyDown(VK_LMENU))
						keyboard.OnKeyReleased(GetSKeyCodes(VK_LMENU));
					else if (!IsKeyDown(VK_RMENU))
						keyboard.OnKeyReleased(GetSKeyCodes(VK_RMENU));

					break;
				}
				case VK_CONTROL:
				{
					if (!IsKeyDown(VK_LCONTROL))
						keyboard.OnKeyReleased(GetSKeyCodes(VK_LCONTROL));
					else if (!IsKeyDown(VK_RCONTROL))
						keyboard.OnKeyReleased(GetSKeyCodes(VK_RCONTROL));

					break;
				}
				}

				keyboard.OnKeyReleased(GetSKeyCodes(rawKeyboard.VKey));
			}
		}

		break;
	}
	/************* END RAW MESSAGES *************/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void WinWindow::SetTitle(const std::string& title)
{
	if (!SetWindowTextA(m_hWnd, title.c_str()))
		throw WIN32_LAST_EXCEPT();
}

std::optional<int> WinWindow::Update()
{
	MSG msg{};

	while (PeekMessageA(&msg, nullptr , 0u, 0u, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return static_cast<int>(msg.wParam);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

void WinWindow::ToggleFullScreenMode()
{
	if (m_fullScreenMode)
	{
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
	else
	{
		GetWindowRect(m_hWnd, &m_windowRect);

		SetWindowLong(
			m_hWnd, GWL_STYLE,
			m_windowStyle & ~(
				WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX |
				WS_SYSMENU)
		);

		// Needed for multi monitor setups
		if (m_pRenderer)
		{
			auto [width, height] = m_pRenderer->GetFirstDisplayCoordinates();

			RECT renderingMonitorCoordinate
			{
				.right  = static_cast<LONG>(width),
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

void WinWindow::EnableCursor() noexcept
{
	m_cursorEnabled = true;

	ShowCursor();
	FreeCursor();
}

void WinWindow::DisableCursor() noexcept
{
	m_cursorEnabled = false;

	HideCursor();
	ConfineCursor();
}

void WinWindow::HideCursor() noexcept
{
	while (::ShowCursor(FALSE) >= 0) {}
}

void WinWindow::ShowCursor() noexcept
{
	while (::ShowCursor(TRUE) < 0) {}
}

void WinWindow::ConfineCursor() noexcept
{
	RECT rect{};

	GetClientRect(m_hWnd, &rect);
	MapWindowPoints(m_hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2u);
	ClipCursor(&rect);
}

void WinWindow::FreeCursor() noexcept
{
	ClipCursor(nullptr);
}

bool WinWindow::IsCursorEnabled() const noexcept
{
	return m_cursorEnabled;
}

void* WinWindow::GetWindowHandle() const noexcept
{
	return reinterpret_cast<void*>(m_hWnd);
}

void* WinWindow::GetModuleInstance() const noexcept
{
	return reinterpret_cast<void*>(m_windowClass.GetHInstance());
}

HICON WinWindow::LoadIconFromPath(const wchar_t* iconPath)
{
	std::wstring relativePath = std::filesystem::current_path().wstring();

	return reinterpret_cast<HICON>(
		LoadImageW(
			nullptr, (relativePath + L"\\" + iconPath).c_str(), IMAGE_ICON, 0, 0,
			LR_DEFAULTSIZE | LR_LOADFROMFILE
		));
}

void WinWindow::SetWindowIcon(const std::wstring& iconPath)
{
	HICON hIcon = LoadIconFromPath(iconPath.c_str());

	SendMessageA(m_hWnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
	SendMessageA(m_hWnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
}

bool WinWindow::IsMinimised() const noexcept
{
	return m_isMinimised;
}

void WinWindow::SetRenderer(std::shared_ptr<Renderer> renderer) noexcept
{
	m_pRenderer = std::move(renderer);
}

void WinWindow::SetInputManager(std::shared_ptr<InputManager> ioMan)
{
	m_pInputManager = std::move(ioMan);

	std::vector<RAWINPUTDEVICE> rawInputIDs{};

	rawInputIDs.emplace_back(
		RAWINPUTDEVICE
		{
			HID_USAGE_PAGE_GENERIC,
			HID_USAGE_GENERIC_KEYBOARD,
			RIDEV_DEVNOTIFY,
			m_hWnd
		}
	);

	rawInputIDs.emplace_back(
		RAWINPUTDEVICE
		{
			HID_USAGE_PAGE_GENERIC,
			HID_USAGE_GENERIC_MOUSE,
			RIDEV_DEVNOTIFY,
			m_hWnd
		}
	);

	size_t gamepadCount = m_pInputManager->GetGamepadCount();

	if (gamepadCount >= 5u)
		WIN32_GENERIC_THROW(
			"Maximum supported XBox gamepads are four."
		);

	for (size_t index = 0u; index < gamepadCount; ++index)
	{
		Gamepad& gamepad = m_pInputManager->GetGamepad(index);

		if (!gamepad.GetLeftThumbStickDeadZone())
			gamepad.SetLeftThumbStickDeadZone(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

		if (!gamepad.GetRightThumbStickDeadZone())
			gamepad.SetRightThumbStickDeadZone(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

		if (!gamepad.GetTriggerThreshold())
			gamepad.SetTriggerThreshold(XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

		rawInputIDs.emplace_back(
			RAWINPUTDEVICE
			{
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

	// RID_INPUT in GetRawInputData returns the sizeof(RAWINPUT) and RID_HEADER returns
	// the sizeof(RAWINPUTHEADER)
	m_rawInputBuffer.resize(sizeof(RAWINPUT));
}

float WinWindow::GetAspectRatio() const noexcept
{
	return static_cast<float>(m_width) / m_height;
}

void WinWindow::UpdateIndependentInputs() const noexcept
{
	CheckXBoxControllerStates(m_pInputManager.get());
}

bool WinWindow::IsKeyDown(int vKey) const noexcept
{
	union
	{
		SHORT _signed;
		unsigned short _unsigned;
	}keyState{ GetAsyncKeyState(vKey) };

	return keyState._unsigned & 0x8000u;
}

void WinWindow::AddInputCallback(
	void(*callback)(void*, std::uint32_t, std::uint64_t, std::uint64_t, void*),
	void* extraData/* = nullptr */
) noexcept {
	m_inputCallbacks.emplace_back(
		CallbackData{
			.callback  = callback,
			.extraData = extraData
		}
	);
}
