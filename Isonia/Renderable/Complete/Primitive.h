#pragma once

// internal
#include "Vertex.h"
#include "Builder.h"
#include "PrimitiveFace.h"
#include "PrimitivePrism.h"
#include "PrimitiveSphere.h"

// std
#include <stdexcept>

namespace Isonia::Renderable::Complete
{
	struct Primitive
	{
		enum Type {
			Triangle,
			Quad,
			Pentagon,
			Hexagon,
			TriangularPrism,
			Cube,
			PentagonalPrism,
			HexagonalPrism,
			Icosahedron,
		};

		static constexpr const std::vector<Vertex> Vertices(const Type type)
		{
			switch (type)
			{
			case Type::Triangle:
				return PrimitiveFace::GenerateFaceVertices(3);
			case Type::Quad:
				return PrimitiveFace::GenerateFaceVertices(4);
			case Type::Pentagon:
				return PrimitiveFace::GenerateFaceVertices(5);
			case Type::Hexagon:
				return PrimitiveFace::GenerateFaceVertices(6);

			case Type::TriangularPrism:
				return PrimitivePrism::GeneratePrismVertices(3);
			case Type::Cube:
				return PrimitivePrism::GeneratePrismVertices(4);
			case Type::PentagonalPrism:
				return PrimitivePrism::GeneratePrismVertices(5);
			case Type::HexagonalPrism:
				return PrimitivePrism::GeneratePrismVertices(6);

			case Type::Icosahedron:
				return PrimitiveSphere::GenerateSphereVertices(1);
			}
			throw std::invalid_argument("Unknown Primitive Type");
		};

		static constexpr const std::vector<uint32_t> Indices(const Type type)
		{
			switch (type)
			{
			case Type::Triangle:
				return PrimitiveFace::GenerateFaceIndices(3);
			case Type::Quad:
				return PrimitiveFace::GenerateFaceIndices(4);
			case Type::Pentagon:
				return PrimitiveFace::GenerateFaceIndices(5);
			case Type::Hexagon:
				return PrimitiveFace::GenerateFaceIndices(6);

			case Type::TriangularPrism:
				return PrimitivePrism::GeneratePrismIndices(3);
			case Type::Cube:
				return PrimitivePrism::GeneratePrismIndices(4);
			case Type::PentagonalPrism:
				return PrimitivePrism::GeneratePrismIndices(5);
			case Type::HexagonalPrism:
				return PrimitivePrism::GeneratePrismIndices(6);

			case Type::Icosahedron:
				return PrimitiveSphere::GenerateSphereIndices(1);

			}
			throw std::invalid_argument("Unknown Primitive Type");
		};
	};
}
