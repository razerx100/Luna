#include <WinWindow.hpp>
#include <WindowThrowMacros.hpp>
#include <cmath>
#include <array>
#include <filesystem>
#include <hidusage.h>

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
) : m_inputCallbacks{}, m_hWnd{ nullptr },
	m_width{ width }, m_height{ height },
	m_windowClass{}, m_windowRect{ 0l, 0l, 0l, 0l },
	m_windowStyle{ WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU },
	m_fullScreenMode{ false }, m_cursorEnabled{ true }, m_isMinimised{ false }
{
	m_windowClass.Register();

	RECT windowRect
	{
		.left   = 0,
		.top    = 0,
		.right  = static_cast<LONG>(width),
		.bottom = static_cast<LONG>(height)
	};

	if (!AdjustWindowRect(&windowRect, m_windowStyle, FALSE))
		throw WIN32_LAST_EXCEPT();

	m_hWnd = CreateWindowExA(
		0ul,
		m_windowClass.GetName(), name,
		m_windowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		static_cast<int>(windowRect.right - windowRect.left),
		static_cast<int>(windowRect.bottom - windowRect.top),
		nullptr, nullptr, m_windowClass.GetHInstance(), this
	);

	if (!m_hWnd)
		throw WIN32_LAST_EXCEPT();

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);

	SetRawDevices();
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
	case WM_SIZE:
	{
		m_isMinimised = wParam == SIZE_MINIMIZED;

		if (!m_cursorEnabled)
			ConfineCursor();

		break;
	}
	case WM_ACTIVATE:
	{
		if (!m_cursorEnabled)
		{
			if (wParam & WA_ACTIVE || wParam & WA_CLICKACTIVE)
				ConfineCursor();
			else
				FreeCursor();
		}

		break;
	}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void WinWindow::SetTitle(const std::string& title)
{
	if (!SetWindowTextA(m_hWnd, title.c_str()))
		throw WIN32_LAST_EXCEPT();
}

std::int32_t WinWindow::Update()
{
	MSG msg{};

	while (PeekMessageA(&msg, nullptr , 0u, 0u, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			// Should be returning 0 here from the PostQuitMessage function.
			return static_cast<std::int32_t>(msg.wParam);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 1;
}

void WinWindow::ToggleFullscreen(std::uint32_t width, std::uint32_t height) noexcept
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
			m_windowStyle & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU)
		);

		RECT renderingMonitorCoordinate
		{
			.left   = 0,
			.top    = 0,
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
	return m_hWnd;
}

void* WinWindow::GetModuleInstance() const noexcept
{
	return m_windowClass.GetHInstance();
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

void WinWindow::SetRawDevices()
{
	std::array rawInputIDs
	{
		RAWINPUTDEVICE
		{
			HID_USAGE_PAGE_GENERIC,
			HID_USAGE_GENERIC_KEYBOARD,
			RIDEV_DEVNOTIFY,
			m_hWnd
		},
		RAWINPUTDEVICE
		{
			HID_USAGE_PAGE_GENERIC,
			HID_USAGE_GENERIC_MOUSE,
			RIDEV_DEVNOTIFY,
			m_hWnd
		}
	};

	if (!RegisterRawInputDevices(
		std::data(rawInputIDs),
		static_cast<UINT>(std::size(rawInputIDs)), static_cast<UINT>(sizeof(RAWINPUTDEVICE))
	))
		throw WIN32_LAST_EXCEPT();
}

float WinWindow::GetAspectRatio() const noexcept
{
	return static_cast<float>(m_width) / m_height;
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
