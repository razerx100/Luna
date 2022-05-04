#ifndef RENDERER_HPP_
#define RENDERER_HPP_
#include <cstdint>
#include <utility>

class Renderer {
public:
	using Resolution = std::pair<std::uint64_t, std::uint64_t>;

	virtual ~Renderer() = default;

	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;

	[[nodiscard]]
	virtual Resolution GetDisplayCoordinates(std::uint32_t displayIndex = 0u) const = 0;
};

#endif
