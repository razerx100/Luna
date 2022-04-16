#ifndef I_GRAPHICS_ENGINE_HPP_
#define I_GRAPHICS_ENGINE_HPP_
#include <cstdint>

class GraphicsEngine {
public:
	virtual ~GraphicsEngine() = default;

	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;
	virtual void GetMonitorCoordinates(
		std::uint64_t& monitorWidth, uint64_t& monitorHeight
	) = 0;
};

#endif
