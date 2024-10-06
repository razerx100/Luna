#include <WinWindow.hpp>
#include <LunaInstance.hpp>

std::unique_ptr<Window> CreateLunaInstance(
	std::uint32_t width, std::uint32_t height, const char* name
) {
	return std::make_unique<WinWindow>(width, height, name);
}
