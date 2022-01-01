#ifndef __LUNA_INSTANCE_HPP__
#define __LUNA_INSTANCE_HPP__
#include <Window.hpp>

[[nodiscard]]
LUNA_DLL Window* __cdecl CreateLunaInstance(
	int width, int height, class InputManager* ioMan, const char* name
);
#endif
