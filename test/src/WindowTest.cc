#include <gtest/gtest.h>
#include <WinWindow.hpp>

namespace Constants
{
	constexpr const char* appName         = "LunaTest";
	static constexpr std::uint32_t width  = 1280u;
	static constexpr std::uint32_t height = 720u;
}

TEST(WindowTest, WinWindowTest)
{
	WinWindow window{ Constants::width, Constants::height, Constants::appName };
}
