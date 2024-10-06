#ifndef RENDERER_HPP_
#define RENDERER_HPP_
#include <cstdint>
#include <utility>

class Renderer {
public:
	struct Resolution {
		std::uint64_t width;
		std::uint64_t height;
	};

	virtual ~Renderer() = default;

	virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;

	[[nodiscard]]
	virtual Resolution GetFirstDisplayCoordinates() const = 0;
};

#endif
