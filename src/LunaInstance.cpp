#include <WinWindow.hpp>
#include <LunaInstance.hpp>

Window* CreateLunaInstance(
	std::uint32_t width, std::uint32_t height, InputManager* ioMan, const char* name
) {
	return new WinWindow(width, height, ioMan, name);
}
