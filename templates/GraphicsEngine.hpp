#ifndef __GRAPHICS_ENGINE_HPP__
#define __GRAPHICS_ENGINE_HPP__
#include <cstdint>

#define GAIAX_DLL __declspec(dllimport)

struct GAIAX_DLL SRect {
	long left;
	long top;
	long right;
	long bottom;
};

struct GAIAX_DLL Color {
	float r;
	float g;
	float b;
	float a;
};

class GAIAX_DLL GraphicsEngine {
public:
	virtual ~GraphicsEngine() = default;

	virtual void SetBackgroundColor(Color color) noexcept = 0;
	virtual void SubmitCommands() = 0;
	virtual void Render() = 0;
	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;
	virtual SRect GetMonitorCoordinates() = 0;
};

GAIAX_DLL GraphicsEngine* __cdecl GetGraphicsEngineInstance() noexcept;
GAIAX_DLL void __cdecl InitGraphicsEngineInstance(
	void* windowHandle, std::uint32_t width, std::uint32_t height,
	std::uint8_t bufferCount
);
GAIAX_DLL void __cdecl CleanUpGraphicsEngineInstance() noexcept;

#endif
