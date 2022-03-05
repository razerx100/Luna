#ifndef __I_GRAPHICS_ENGINE_HPP__
#define __I_GRAPHICS_ENGINE_HPP__
#include <cstdint>

namespace Ceres {
	template<typename T>
	struct _struct4;
	typedef _struct4<float> Float32_4;
}

class __declspec(dllimport) GraphicsEngine {
public:
	virtual ~GraphicsEngine() = default;

	[[nodiscard]] // Returns index of the resource in Resource Heap
	virtual size_t RegisterResource(
		const void* data, size_t rowPitch, size_t rows
	) = 0;

	virtual void SetBackgroundColour(const Ceres::Float32_4& colourVector) noexcept = 0;
	virtual void SubmitModel(const class IModel* const modelRef) = 0;
	virtual void Render() = 0;
	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;
	virtual void GetMonitorCoordinates(
		std::uint64_t& monitorWidth, uint64_t& monitorHeight
	) = 0;
	virtual void WaitForAsyncTasks() = 0;

	virtual void SetShaderPath(const char* path) noexcept = 0;
	virtual void InitResourceBasedObjects() = 0;
	virtual void ProcessData() = 0;
};

#endif
