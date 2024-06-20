// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	static constexpr const std::vector<VertexComplete> generatePrimitiveSphereVertices(const unsigned int sub_divisions)
	{
		constexpr const float X = 0.525731112119133606f;
		constexpr const float Z = 0.850650808352039932f;
		constexpr const float N = 0.f;

		constexpr const unsigned int VERTICE_COUNT = 4 * 3;
		constexpr const Math::Vector3 VERTICES[VERTICE_COUNT] =
		{
			{-X,N,Z}, { X,N, Z}, {-X, N,-Z}, { X, N,-Z},
			{ N,Z,X}, { N,Z,-X}, { N,-Z, X}, { N,-Z,-X},
			{ Z,X,N}, {-Z,X, N}, { Z,-X, N}, {-Z,-X, N}
		};

		assert(sub_divisions > 0 && "Invalid number of sub divisions");

		std::vector<VertexComplete> vertices(VERTICES, VERTICES + VERTICE_COUNT);

		return vertices;
	}

	static constexpr const std::vector<unsigned int> generatePrimitiveSphereIndices(const unsigned int sub_divisions)
	{
		constexpr const unsigned int TRIANGLE_COUNT = 5 * 4 * 3;
		constexpr const unsigned int TRIANGLES[TRIANGLE_COUNT] =
		{
			0,4,1,  0,9,4,  9,5,4,  4,5,8,  4,8,1,
			8,10,1, 8,3,10, 5,3,8,  5,2,3,  2,7,3,
			7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6,
			6,1,10, 9,0,11, 9,11,2, 9,2,5,  7,2,11
		};

		assert(sub_divisions > 0 && "Invalid number of sub divisions");

		std::vector<unsigned int> indices(TRIANGLES, TRIANGLES + TRIANGLE_COUNT);

		return indices;
	}
}
