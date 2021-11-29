#include <WinWindow.hpp>
#include <WindowThrowMacros.hpp>
#include <filesystem>
#include <IGraphicsEngine.hpp>
#include <hidusage.h>
#include <Xinput.h>

#ifdef _IMGUI
#include <imgui_impl_win32.h>
// Forward Declaration of ImGui wndproc
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

WinWindow::WindowClass::WindowClass() noexcept
	: m_wndClass{} {

	m_wndClass.cbSize = sizeof(m_wndClass);
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
WinWindow::WinWindow(int width, int height, const char* name)
	: m_width(width), m_height(height), m_fullScreenMode(false),
	m_windowStyle(WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU),
	m_cursorEnabled(true), m_isMinimized(false) {

	m_windowClass.Register();

	RECT wr;
	wr.left = 0;
	wr.right = width;
	wr.top = 0;
	wr.bottom = height;
	if (!AdjustWindowRect(&wr, m_windowStyle, FALSE))
		throw HWND_LAST_EXCEPT();

	m_hWnd = CreateWindowEx(
		0,
		m_windowClass.GetName(), name,
		m_windowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, m_windowClass.GetHInstance(), this
	);

	if (!m_hWnd)
		throw HWND_LAST_EXCEPT();

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);

	if ((m_pInputManagerRef = GetInputManagerInstance()) == nullptr)
		throw GenericException(
			__LINE__, __FILE__,
			"No Input Manager Object Initialized."
		);

#ifdef _IMGUI
	ImGui_ImplWin32_Init(m_hWnd);
#endif
	std::vector<RAWINPUTDEVICE> rIDs;
	std::uint32_t keyboardCount = m_pInputManagerRef->GetKeyboardCount();
	std::uint32_t mouseCount = m_pInputManagerRef->GetMouseCount();
	std::uint32_t gamepadCount = m_pInputManagerRef->GetGamepadCount();

	if (gamepadCount >= 5u)
		throw GenericException(
			__LINE__, __FILE__,
			"Maximum supported XBox gamepads are four."
		);

	for (std::uint32_t index = 0u;
		index < keyboardCount;
		++index)
		rIDs.emplace_back(
			RAWINPUTDEVICE{
				HID_USAGE_PAGE_GENERIC,
				HID_USAGE_GENERIC_KEYBOARD,
				RIDEV_DEVNOTIFY,
				m_hWnd
			}
		);

	for(std::uint32_t index = 0u;
		index < mouseCount;
		++index)
		rIDs.emplace_back(
			RAWINPUTDEVICE{
				HID_USAGE_PAGE_GENERIC,
				HID_USAGE_GENERIC_MOUSE,
				RIDEV_DEVNOTIFY,
				m_hWnd
			}
		);

	std::vector<IGamepad*> pGamepadRefs = m_pInputManagerRef->GetGamepadRefs();
	for (IGamepad* gamepad : pGamepadRefs) {
		if(!gamepad->GetLeftThumbStickDeadZone())
			gamepad->SetLeftThumbStickDeadZone(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

		if(!gamepad->GetRightThumbStickDeadZone())
			gamepad->SetRightThumbStickDeadZone(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

		if (!gamepad->GetTriggerThreshold())
			gamepad->SetTriggerThreshold(XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

		rIDs.emplace_back(
			RAWINPUTDEVICE{
				HID_USAGE_PAGE_GENERIC,
				HID_USAGE_GENERIC_GAMEPAD,
				RIDEV_DEVNOTIFY,
				m_hWnd
			}
		);
	}

	if (!RegisterRawInputDevices(
		rIDs.data(), static_cast<std::uint32_t>(rIDs.size()), sizeof(RAWINPUTDEVICE)
	))
		throw HWND_LAST_EXCEPT();
}

WinWindow::~WinWindow() noexcept {
#ifdef _IMGUI
	ImGui_ImplWin32_Shutdown();
#endif
}

LRESULT CALLBACK WinWindow::HandleMsgSetup(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
) noexcept {
	if (msg == WM_NCCREATE) {
		const CREATESTRUCTA* const pCreate = reinterpret_cast<CREATESTRUCTA*>(lParam);
		WinWindow* const pWnd = static_cast<WinWindow*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(
			&WinWindow::HandleMsgWrap)
		);

		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProcA(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WinWindow::HandleMsgWrap(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
) noexcept {
	WinWindow* const pWnd = reinterpret_cast<WinWindow*>(
		GetWindowLongPtr(hWnd, GWLP_USERDATA)
		);
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT WinWindow::HandleMsg(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
) noexcept {
#ifdef _IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	auto imIO = ImGui::GetIO();
#endif

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
		InputManager* inputRef = GetInputManagerInstance();

		if (inputRef)
			inputRef->ClearInputStates();

		break;
	}
	case WM_SIZE: {
		RECT clientRect = {};
		GetClientRect(m_hWnd, &clientRect);

		if (wParam != SIZE_MINIMIZED) {
			m_isMinimized = false;
			m_width = clientRect.right - clientRect.left;
			m_height = clientRect.bottom - clientRect.top;

			if (GetGraphicsEngineInstance())
				GetGraphicsEngineInstance()->Resize(m_width, m_height);
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
		if (wParam == GIDC_REMOVAL)
			GetInputManagerInstance()->DeviceDisconnected(
				static_cast<std::uint64_t>(lParam)
			);

		break;
	}
	/************* KEYBOARD MESSAGES *************/
	case WM_SYSKEYDOWN: {
		if ((wParam == VK_RETURN) && (lParam & (1 << 29)))
			ToggleFullScreenMode();

		break;
	}
	case WM_CHAR: {
#ifdef _IMGUI
		if (imIO.WantCaptureKeyboard)
			break;
#endif

		GetInputManagerInstance()->GetKeyboardByIndex()->OnChar(
			static_cast<char>(wParam)
		);

		break;
	}
	/************* END KEYBOARD MESSAGES *************/
	/************* RAW MOUSE MESSAGES *************/
	case WM_INPUT: {
		std::uint32_t size = 0;

		// Get raw data size with nullptr as buffer
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size,
			sizeof(RAWINPUTHEADER)
		) == -1)
			break;

		m_rawInputBuffer.resize(size);

		// Get raw data by passing buffer
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
			m_rawInputBuffer.data(), &size,
			sizeof(RAWINPUTHEADER)
		) != size)
			break;

		auto& ri = reinterpret_cast<const RAWINPUT&>(*m_rawInputBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE) {
			IMouse* pMouseRef = m_pInputManagerRef->GetMouseByHandle(
				reinterpret_cast<std::uint64_t>(ri.header.hDevice)
			);

			if (ri.data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
				pMouseRef->OnWheelDelta(static_cast<short>(ri.data.mouse.usButtonData));
			else if (ri.data.mouse.usButtonFlags) {
				auto processedData = ProcessMouseRawButtons(ri.data.mouse.usButtonFlags);
				pMouseRef->SetPressState(processedData.first);
				pMouseRef->SetReleaseState(processedData.second);
			}

			if (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0)
				pMouseRef->OnMouseMove(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
		}
		else if (ri.header.dwType == RIM_TYPEKEYBOARD) {
			IKeyboard* pKeyboardRef = m_pInputManagerRef->GetKeyboardByHandle(
				reinterpret_cast<std::uint64_t>(ri.header.hDevice)
			);

			std::uint32_t legacyMessage = ri.data.keyboard.Message;
			if (legacyMessage == WM_KEYDOWN || legacyMessage == WM_SYSKEYDOWN)
				pKeyboardRef->OnKeyPressed(
					GetSKeyCodes(
						ri.data.keyboard.VKey
					)
				);
			else if (legacyMessage == WM_KEYUP || legacyMessage == WM_SYSKEYUP)
				pKeyboardRef->OnKeyReleased(
					GetSKeyCodes(
						ri.data.keyboard.VKey
					)
				);
		}
		else if (ri.header.dwType == RIM_TYPEHID) {
			GamepadData pGamepadRef = m_pInputManagerRef->GetGamepadByHandle(
				reinterpret_cast<std::uint64_t>(ri.header.hDevice)
			);

			XINPUT_STATE state;
			ZeroMemory(&state, sizeof(XINPUT_STATE));

			if (XInputGetState(pGamepadRef.index, &state) == ERROR_SUCCESS) {
				XINPUT_GAMEPAD xData = state.Gamepad;

				pGamepadRef.pGamepad->SetRawButtonState(
					ProcessGamepadRawButtons(
						xData.wButtons
					)
				);

				std::uint16_t leftStickDeadZone = pGamepadRef.pGamepad->
					GetLeftThumbStickDeadZone();
				if (float magnitude = GetMagnitude(xData.sThumbLX, xData.sThumbLY);
					magnitude > leftStickDeadZone)
					pGamepadRef.pGamepad->OnLeftThumbStickMove(
						ProcessASMagnitude(
							magnitude, xData.sThumbLX, xData.sThumbLY,
							leftStickDeadZone
						)
					);

				std::uint16_t rightStickDeadZone = pGamepadRef.pGamepad->
					GetRightThumbStickDeadZone();
				if (float magnitude = GetMagnitude(xData.sThumbRX, xData.sThumbRY);
					magnitude > rightStickDeadZone)
					pGamepadRef.pGamepad->OnRightThumbStickMove(
						ProcessASMagnitude(
							magnitude, xData.sThumbRX, xData.sThumbRY,
							rightStickDeadZone
						)
					);

				std::uint32_t threshold = pGamepadRef.pGamepad->GetTriggerThreshold();
				if (xData.bLeftTrigger > threshold)
					pGamepadRef.pGamepad->OnLeftTriggerMove(
						ProcessDeadZone(
							static_cast<float>(xData.bLeftTrigger),
							255u,
							threshold
						)
					);

				if (xData.bRightTrigger > threshold)
					pGamepadRef.pGamepad->OnRightTriggerMove(
						ProcessDeadZone(
							static_cast<float>(xData.bRightTrigger),
							255u,
							threshold
						)
					);
			}
		}

		break;
	}
	/************* END RAW MOUSE MESSAGES *************/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void WinWindow::SetTitle(const char* title) {
	if (!SetWindowTextA(m_hWnd, title))
		throw HWND_LAST_EXCEPT();
}

int WinWindow::Update() {
	MSG msg = {};

	while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
			return static_cast<int>(msg.wParam);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 1;
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
		if (GetGraphicsEngineInstance()) {
			RECT renderingMonitorCoordinate = {};

			std::uint64_t width = 0u;
			std::uint64_t height = 0u;
			GetGraphicsEngineInstance()->GetMonitorCoordinates(
				width, height
			);

			renderingMonitorCoordinate.right = static_cast<std::int64_t>(width);
			renderingMonitorCoordinate.bottom = static_cast<std::int64_t>(height);

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

#ifdef _IMGUI
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
#endif
}

void WinWindow::DisableCursor() noexcept {
	m_cursorEnabled = false;

	HideCursor();
	ConfineCursor();

#ifdef _IMGUI
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
#endif
}

void WinWindow::HideCursor() noexcept {
	while (::ShowCursor(FALSE) >= 0);
}

void WinWindow::ShowCursor() noexcept {
	while (::ShowCursor(TRUE) < 0);
}

void WinWindow::ConfineCursor() noexcept {
	RECT rect = {};
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

HICON WinWindow::LoadIconFromPath(const char* iconPath) {
	std::string relativePath = std::filesystem::current_path().string();

	return reinterpret_cast<HICON>(
		LoadImageA(
			nullptr, (relativePath + "\\" + iconPath).c_str(), IMAGE_ICON, 0, 0,
			LR_DEFAULTSIZE | LR_LOADFROMFILE
		));
}

void WinWindow::SetWindowIcon(const char* iconPath) {
	HICON hIcon = LoadIconFromPath(iconPath);

	SendMessageA(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	SendMessageA(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
}

float WinWindow::GetMagnitude(std::int16_t x, std::int16_t y) const noexcept {
	return std::sqrtf(
		static_cast<float>(std::pow(x, 2) +
		std::pow(y, 2))
	);
}

float WinWindow::ProcessDeadZone(
	float magnitude, std::uint32_t maxValue, std::uint32_t deadZone
) const noexcept {
	magnitude = std::min(magnitude, static_cast<float>(maxValue));
	magnitude -= deadZone;

	return magnitude / (maxValue - deadZone);
}

ASData WinWindow::ProcessASMagnitude(
	float magnitude, std::int16_t x, std::int16_t y, std::uint32_t deadZone
) const noexcept {
	ASData data = {};

	data.xDirection = x / magnitude;
	data.yDirection = y / magnitude;

	data.magnitude = ProcessDeadZone(magnitude, 32767u, deadZone);

	return data;
}

bool WinWindow::IsMinimized() const noexcept {
	return m_isMinimized;
}
