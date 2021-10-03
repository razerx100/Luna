#ifndef __GRAPHICS_ENGINE_HPP__
#define __GRAPHICS_ENGINE_HPP__
#include <cstdint>

struct __declspec(dllimport) SRect {
	long left;
	long top;
	long right;
	long bottom;
};

class __declspec(dllimport) GraphicsEngine {
public:
	virtual ~GraphicsEngine() = default;

	virtual void SubmitCommands() = 0;
	virtual void Render() = 0;
	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;
	virtual SRect GetMonitorCoordinates() = 0;
};

__declspec(dllimport) GraphicsEngine* _cdecl GetGraphicsEngineInstance() noexcept;
__declspec(dllimport) void _cdecl InitGraphicsEngineInstance(void* windowHandle);
__declspec(dllimport) void _cdecl CleanUpGraphicsEngineInstance() noexcept;

#endif
