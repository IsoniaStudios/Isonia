// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	extern constexpr const std::vector<VertexComplete> generatePrimitiveVertices(const PrimitiveType type)
	{
		switch (type)
		{
		case PrimitiveType::Triangle:
			return generatePrimitiveFaceVertices(3);
		case PrimitiveType::Quad:
			return generatePrimitiveFaceVertices(4);
		case PrimitiveType::Pentagon:
			return generatePrimitiveFaceVertices(5);
		case PrimitiveType::Hexagon:
			return generatePrimitiveFaceVertices(6);

		case PrimitiveType::TriangularPrism:
			return generatePrimitivePrismVertices(3);
		case PrimitiveType::Cube:
			return generatePrimitivePrismVertices(4);
		case PrimitiveType::PentagonalPrism:
			return generatePrimitivePrismVertices(5);
		case PrimitiveType::HexagonalPrism:
			return generatePrimitivePrismVertices(6);

		case PrimitiveType::Icosahedron:
			return generatePrimitiveSphereVertices(1);
		}
		throw std::invalid_argument("Unknown Primitive Type");
	};

	extern constexpr const std::vector<unsigned int> generatePrimitiveIndices(const PrimitiveType type)
	{
		switch (type)
		{
		case PrimitiveType::Triangle:
			return generatePrimitiveFaceIndices(3);
		case PrimitiveType::Quad:
			return generatePrimitiveFaceIndices(4);
		case PrimitiveType::Pentagon:
			return generatePrimitiveFaceIndices(5);
		case PrimitiveType::Hexagon:
			return generatePrimitiveFaceIndices(6);

		case PrimitiveType::TriangularPrism:
			return generatePrimitivePrismIndices(3);
		case PrimitiveType::Cube:
			return generatePrimitivePrismIndices(4);
		case PrimitiveType::PentagonalPrism:
			return generatePrimitivePrismIndices(5);
		case PrimitiveType::HexagonalPrism:
			return generatePrimitivePrismIndices(6);

		case PrimitiveType::Icosahedron:
			return generatePrimitiveSphereIndices(1);
		}
		throw std::invalid_argument("Unknown Primitive Type");
	};
}
