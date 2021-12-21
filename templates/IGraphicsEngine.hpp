#ifndef __I_GRAPHICS_ENGINE_HPP__
#define __I_GRAPHICS_ENGINE_HPP__
#include <cstdint>

namespace Ceres {
	struct VectorF32;
}

class __declspec(dllimport) GraphicsEngine {
public:
	virtual ~GraphicsEngine() = default;

	virtual void SetBackgroundColor(const Ceres::VectorF32& colorVector) noexcept = 0;
	virtual void SubmitModel(const class IModel* const modelRef, bool texture = true) = 0;
	virtual void Render() = 0;
	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;
	virtual void GetMonitorCoordinates(
		std::uint64_t& monitorWidth, uint64_t& monitorHeight
	) = 0;
	virtual void WaitForAsyncTasks() = 0;

	virtual void SetShaderPath(const char* path) noexcept = 0;
	virtual void InitResourceBasedObjects() = 0;
};

#endif
