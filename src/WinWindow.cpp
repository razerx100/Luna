#include <WinWindow.hpp>
#include <WindowThrowMacros.hpp>
#include <PipelineManager.hpp>
#include <filesystem>

#ifdef _IMGUI
#include <imgui_impl_win32.h>
// Forward Declaration of ImGui wndproc
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

// WindowClass
WinWindow::WindowClass WinWindow::WindowClass::s_wndClass;

WinWindow::WindowClass::WindowClass() noexcept
	: m_hInst(GetModuleHandle(nullptr)) {

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;

	RegisterClassEx(&wc);
}

WinWindow::WindowClass::~WindowClass() {
	UnregisterClass(GetName(), GetInstance());
}

const char* WinWindow::WindowClass::GetName() noexcept {
	return wndClassName;
}

HINSTANCE WinWindow::WindowClass::GetInstance() noexcept {
	return s_wndClass.m_hInst;
}

// Window
WinWindow::WinWindow(int width, int height, const char* name)
	: m_width(width), m_height(height), m_fullScreenMode(false),
	m_windowStyle(WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU),
	m_cursorEnabled(true) {
	RECT wr;
	wr.left = 0;
	wr.right = width;
	wr.top = 0;
	wr.bottom = height;
	if (!AdjustWindowRect(&wr, m_windowStyle, FALSE))
		throw HWND_LAST_EXCEPT();

	m_hWnd = CreateWindowEx(
		0,
		WindowClass::GetName(), name,
		m_windowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	if (!m_hWnd)
		throw HWND_LAST_EXCEPT();

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);

	if ((m_kbRef = Keyboard::GetRef()) == nullptr)
		throw GenericException(__LINE__, __FILE__, "No Keyboard Object Initialized.");

	if ((m_mouseRef = Mouse::GetRef()) == nullptr)
		throw GenericException(__LINE__, __FILE__, "No Mouse Object Initialized.");

#ifdef _IMGUI
	ImGui_ImplWin32_Init(m_hWnd);
#endif

	// Raw Input register
	RAWINPUTDEVICE rId = {};
	rId.usUsagePage = 1u;
	rId.usUsage = 2u;
	rId.dwFlags = 0;
	rId.hwndTarget = nullptr;

	if (!RegisterRawInputDevices(&rId, 1, sizeof(rId)))
		throw HWND_LAST_EXCEPT();
}

WinWindow::~WinWindow() {
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
		m_kbRef->ClearState();
		break;
	}
	case WM_SIZE: {
		RECT clientRect = {};
		GetClientRect(m_hWnd, &clientRect);

		m_width = clientRect.right - clientRect.left;
		m_height = clientRect.bottom - clientRect.top;

		if (PipelineManager::GetRef())
			PipelineManager::GetRef()->Resize(m_width, m_height);

		if(!m_cursorEnabled)
			ConfineCursor();
		break;
	}
	case WM_ACTIVATE: {
		if (!m_cursorEnabled) {
			if (wParam & WA_ACTIVE || wParam & WA_CLICKACTIVE) {
				HideCursor();
				ConfineCursor();
			}
			else {
				ShowCursor();
				FreeCursor();
			}
		}
		break;
	}
	/************* KEYBOARD MESSAGES *************/
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureKeyboard)
			break;
#endif
		if ((wParam == VK_RETURN) && (lParam & (1 << 29)))
			ToggleFullScreenMode();

		if (!(lParam & 0x40000000) || m_kbRef->IsAutoRepeatEnabled()) // filters autoRepeat
			m_kbRef->OnKeyPressed(static_cast<unsigned char>(wParam));
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureKeyboard)
			break;
#endif

		m_kbRef->OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	}
	case WM_CHAR: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureKeyboard)
			break;
#endif

		m_kbRef->OnChar(static_cast<char>(wParam));
		break;
	}
	/************* END KEYBOARD MESSAGES *************/
	/************* MOUSE MESSAGES *************/
	case WM_MOUSEMOVE: {
		if (!m_cursorEnabled && !m_mouseRef->IsInWindow()) {
			SetCapture(m_hWnd);
			m_mouseRef->OnMouseEnter();
			HideCursor();
			break;
		}

#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		const POINTS pt = MAKEPOINTS(lParam);
		// In client region
		if (pt.x >= 0 && pt.x < m_width && pt.y >= 0 && pt.y < m_height) {
			m_mouseRef->OnMouseMove(pt.x, pt.y);

			if (!m_mouseRef->IsInWindow()) {
				SetCapture(m_hWnd);
				m_mouseRef->OnMouseEnter();
			}
		}
		// Not in client region, log move if button down
		else {
			if(wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
				m_mouseRef->OnMouseMove(pt.x, pt.y);
			else {
				ReleaseCapture();
				m_mouseRef->OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN: {
		SetForegroundWindow(m_hWnd);

#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		m_mouseRef->OnLeftPress();
		break;
	}
	case WM_LBUTTONUP: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		m_mouseRef->OnLeftRelease();
		break;
	}
	case WM_MBUTTONDOWN: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		m_mouseRef->OnMiddlePress();
		break;
	}
	case WM_MBUTTONUP: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		m_mouseRef->OnMiddleRelease();
		break;
	}
	case WM_RBUTTONDOWN: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		m_mouseRef->OnRightPress();
		break;
	}
	case WM_RBUTTONUP: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		m_mouseRef->OnRightRelease();
		break;
	}
	case WM_MOUSEWHEEL: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		int deltaWparam = GET_WHEEL_DELTA_WPARAM(wParam);

		m_mouseRef->OnWheelDelta(deltaWparam);
		break;
	}
	/************* END MOUSE MESSAGES *************/
	/************* RAW MOUSE MESSAGES *************/
	case WM_INPUT: {
		if (!m_mouseRef->IsRawEnabled())
			break;

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
		if (ri.header.dwType == RIM_TYPEMOUSE &&
			(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
			m_mouseRef->OnMouseRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);

		break;
	}
	/************* END RAW MOUSE MESSAGES *************/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void WinWindow::SetTitle(const std::string& title) {
	if (!SetWindowText(m_hWnd, title.c_str()))
		throw HWND_LAST_EXCEPT();
}

std::optional<int> WinWindow::Update() {
	MSG msg = {};

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
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

		RECT fullscreenWindowRect = {};

		if (PipelineManager::GetRef()) {
			SRect sRect = PipelineManager::GetRef()->GetRenderAreaRECT();
			fullscreenWindowRect = *reinterpret_cast<RECT*>(&sRect);
		}

		SetWindowPos(
			m_hWnd,
			HWND_TOPMOST,
			fullscreenWindowRect.left,
			fullscreenWindowRect.top,
			fullscreenWindowRect.right,
			fullscreenWindowRect.bottom,
			SWP_FRAMECHANGED | SWP_NOACTIVATE
		);

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

HICON WinWindow::LoadIconFromPath(const std::string& iconPath) {
	std::string relativePath = std::filesystem::current_path().string();

	return reinterpret_cast<HICON>(
		LoadImageA(
			nullptr, (relativePath + "\\" + iconPath).c_str(), IMAGE_ICON, 0, 0,
			LR_DEFAULTSIZE | LR_LOADFROMFILE
		));
}

void WinWindow::SetWindowIcon(const std::string& iconPath) {
	HICON hIcon = LoadIconFromPath(iconPath);

	SendMessageA(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	SendMessageA(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
}
