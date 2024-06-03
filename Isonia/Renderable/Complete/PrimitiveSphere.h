#pragma once

// internal
#include "Vertex.h"
#include "Builder.h"
#include "PrimitiveFace.h"
#include "../../Math/Vector.h"

// std
#include <stdexcept>

namespace Isonia::Renderable::Complete
{
	struct PrimitiveSphere
	{
	private:
		static constexpr const float X = 0.525731112119133606f;
		static constexpr const float Z = 0.850650808352039932f;
		static constexpr const float N = 0.f;

		static constexpr const uint32_t VERTICE_COUNT = 4 * 3;
		static constexpr const Math::Vector3 VERTICES[VERTICE_COUNT] =
		{
			{-X,N,Z}, { X,N, Z}, {-X, N,-Z}, { X, N,-Z},
			{ N,Z,X}, { N,Z,-X}, { N,-Z, X}, { N,-Z,-X},
			{ Z,X,N}, {-Z,X, N}, { Z,-X, N}, {-Z,-X, N}
		};

		static constexpr const uint32_t TRIANGLE_COUNT = 5 * 4 * 3;
		static constexpr const uint32_t TRIANGLES[TRIANGLE_COUNT] =
		{
			0,4,1,  0,9,4,  9,5,4,  4,5,8,  4,8,1,
			8,10,1, 8,3,10, 5,3,8,  5,2,3,  2,7,3,
			7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6,
			6,1,10, 9,0,11, 9,11,2, 9,2,5,  7,2,11
		};

	public:
		static constexpr const std::vector<Vertex> GenerateSphereVertices(const uint32_t subDivisions)
		{
			assert(subDivisions > 0 && "Invalid number of subdivisions");

			std::vector<Vertex> vertices(VERTICES, VERTICES + VERTICE_COUNT);

			return vertices;
		}

		static constexpr const std::vector<uint32_t> GenerateSphereIndices(const uint32_t subDivisions)
		{
			assert(subDivisions > 0 && "Invalid number of subdivisions");

			std::vector<uint32_t> indices(TRIANGLES, TRIANGLES + TRIANGLE_COUNT);

			return indices;
		}
	};
}
