#ifndef __GRAPHICS_ENGINE_HPP__
#define __GRAPHICS_ENGINE_HPP__
#include <cstdint>

class __declspec(dllimport) GraphicsEngine {
public:
	virtual ~GraphicsEngine() = default;

	virtual void SetBackgroundColor(const float* colorVector) noexcept = 0;
	virtual void SubmitModels(const class IModel* const models, std::uint32_t modelCount) = 0;
	virtual void Render() = 0;
	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;
	virtual void GetMonitorCoordinates(std::uint64_t* rect4Elements) = 0;
	virtual void WaitForAsyncTasks() = 0;
};

__declspec(dllimport) GraphicsEngine* __cdecl GetGraphicsEngineInstance() noexcept;
__declspec(dllimport) void __cdecl InitGraphicsEngineInstance(
	const char* appName,
	void* windowHandle,
	void* moduleHandle,
	std::uint32_t width, std::uint32_t height,
	std::uint8_t bufferCount = 2u
);
__declspec(dllimport) void __cdecl CleanUpGraphicsEngineInstance() noexcept;

#endif
