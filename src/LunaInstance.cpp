#include <WinWindow.hpp>
#include <LunaInstance.hpp>

Window* CreateLunaInstance(
	int width, int height, InputManager* ioMan, const char* name
) {
	return new WinWindow(width, height, ioMan, name);
}
