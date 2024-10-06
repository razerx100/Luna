#include <WinWindow.hpp>
#include <LunaInstance.hpp>

Window* CreateLunaInstance(std::uint32_t width, std::uint32_t height, const char* name) {
	return new WinWindow(width, height, name);
}
