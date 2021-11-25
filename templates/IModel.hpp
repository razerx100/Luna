#ifndef __I_MODEL_HPP__
#define __I_MODEL_HPP__
#include <cstdint>
#include <DirectXMath.h>

class IModel {
public:
	virtual ~IModel() = default;

	virtual void GetVertices(
		DirectX::XMFLOAT3* vertices, std::uint32_t vertexCount
	) const noexcept = 0;
	virtual void GetIndices(
		std::uint16_t* indices, std::uint32_t indexCount
	) const noexcept = 0;
	virtual DirectX::XMMATRIX GetTransform() const noexcept = 0;
};
#endif
