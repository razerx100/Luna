#ifndef LUNA_INSTANCE_HPP_
#define LUNA_INSTANCE_HPP_
#include <Window.hpp>

#ifdef BUILD_LUNA
#define LUNA_DLL __declspec(dllexport)
#else
#define LUNA_DLL __declspec(dllimport)
#endif

[[nodiscard]]
LUNA_DLL Window* __cdecl CreateLunaInstance(
	std::uint32_t width, std::uint32_t height, const char* name
);
#endif
