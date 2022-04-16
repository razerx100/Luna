#ifndef RENDERER_HPP_
#define RENDERER_HPP_
#include <cstdint>

class Renderer {
public:
	virtual ~Renderer() = default;

	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;
	virtual void GetMonitorCoordinates(
		std::uint64_t& monitorWidth, uint64_t& monitorHeight
	) = 0;
};

#endif
