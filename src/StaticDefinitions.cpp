#include <WinWindow.hpp>

static std::unique_ptr<Window> sWindow;

Window* GetWindowInstance() noexcept {
	return sWindow.get();
}

void InitWindowInstance(
	int width, int height, const char* name
) noexcept {
	sWindow = std::make_unique<WinWindow>(width, height, name);
}
