#ifndef __GRAPHICS_ENGINE_HPP__
#define __GRAPHICS_ENGINE_HPP__
#include <cstdint>

struct __declspec(dllimport) SRect {
	long left;
	long top;
	long right;
	long bottom;
};

struct __declspec(dllimport) Color {
	float r;
	float g;
	float b;
	float a;
};

class __declspec(dllimport) GraphicsEngine {
public:
	virtual ~GraphicsEngine() = default;

	virtual void SetBackgroundColor(Color color) noexcept = 0;
	virtual void SubmitCommands() = 0;
	virtual void Render() = 0;
	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;
	virtual SRect GetMonitorCoordinates() = 0;
};

__declspec(dllimport) GraphicsEngine* __cdecl GetGraphicsEngineInstance() noexcept;
__declspec(dllimport) void __cdecl InitGraphicsEngineInstance(
	void* windowHandle,
	void* moduleHandle,
	std::uint32_t width, std::uint32_t height,
	std::uint8_t bufferCount = 2u
);
__declspec(dllimport) void __cdecl CleanUpGraphicsEngineInstance() noexcept;

#endif
