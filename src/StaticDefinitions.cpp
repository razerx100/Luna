#include <WinWindow.hpp>

static Window* s_pWindow = nullptr;

Window* GetWindowInstance() noexcept {
	return s_pWindow;
}

void InitWindowInstance(
	int width, int height, const char* name
) {
	if (!s_pWindow)
		s_pWindow = new WinWindow(width, height, name);
}

void CleanUpWindowInstance() noexcept {
	if (s_pWindow) {
		delete s_pWindow;
		s_pWindow = nullptr;
	}
}
