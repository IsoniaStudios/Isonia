// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	extern constexpr const std::vector<VertexComplete> generatePrimitiveFaceVertices(const unsigned int num_sides, const float height = 0.0f)
	{
		assert(num_sides > 2 && "Invalid number of sides");

		const float angleIncrement = Math::two_pi / static_cast<float>(num_sides);
		const float radius = 0.5f;

		const Math::Vector3 normal = height == 0.0f ? Math::Vector3{ 0.0f, -1.0f, 0.0f } : Math::vec3Normalize(0.0f, height, 0.0f);
		const Math::Vector3 color = Math::Vector3{ 0.0f, 0.0f, 0.0f };

		std::vector<VertexComplete> vertices;
		const float angleOffset = num_sides % 2 == 0 ? angleIncrement * 0.5f : 0.0f;
		for (unsigned int i = 0; i < num_sides; i++)
		{
			const float z = radius * Math::cosf(static_cast<float>(i) * angleIncrement + angleOffset);
			const float x = radius * Math::sinf(static_cast<float>(i) * angleIncrement + angleOffset);
			const Math::Vector3 position{ x, height, z };
			const Math::Vector2 uv{ x + radius, z + radius };
			vertices.push_back({ position, position, normal, uv, });
		}

		return vertices;
	}

	extern constexpr const std::vector<unsigned int> generatePrimitiveFaceIndices(const unsigned int num_sides, const unsigned int offset = 0, const bool faceUp = true)
	{
		assert(num_sides > 2 && "Invalid number of sides");

		const unsigned int numTriangles = num_sides - 2;

		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < numTriangles; i++)
		{
			indices.push_back(0 + offset);
			if (faceUp)
			{
				indices.push_back(i + 2 + offset);
				indices.push_back(i + 1 + offset);
			}
			else
			{
				indices.push_back(i + 1 + offset);
				indices.push_back(i + 2 + offset);
			}
		}

		return indices;
	}

	extern constexpr const std::vector<VertexComplete> generatePrimitivePrismVertices(const unsigned int num_sides)
	{
		assert(num_sides > 2 && "Invalid number of sides");

		std::vector<VertexComplete> vertices;
		const float angleIncrement = Math::two_pi / num_sides;
		const float radius = 0.5f;

		std::vector<VertexComplete> verticesTop = generatePrimitiveFaceVertices(num_sides, -0.5f);
		std::vector<VertexComplete> verticesBottom = generatePrimitiveFaceVertices(num_sides, 0.5f);

		vertices.insert(vertices.end(), verticesTop.begin(), verticesTop.end());
		vertices.insert(vertices.end(), verticesBottom.begin(), verticesBottom.end());

		// Add vertices for the side faces
		for (unsigned int i = 0; i < num_sides; i++)
		{
			const unsigned int nextIndex = (i + 1) % num_sides;

			VertexComplete v1 = { verticesTop[i] };
			VertexComplete v2 = { verticesTop[nextIndex] };
			VertexComplete v3 = { verticesBottom[i] };
			VertexComplete v4 = { verticesBottom[nextIndex] };

			const Math::Vector3 normal = Math::vec3Normalize(
				(v1.position.x + v2.position.x) * 0.5f,
				0.0f,
				(v1.position.z + v2.position.z) * 0.5f
			);
			v1.normal = normal;
			v2.normal = normal;
			v3.normal = normal;
			v4.normal = normal;

			const float uvX1 = static_cast<float>(i) / static_cast<float>(num_sides);
			const float uvX2 = static_cast<float>(i + 1) / static_cast<float>(num_sides);
			v1.uv = { uvX1, 1.0f };
			v2.uv = { uvX2, 1.0f };
			v3.uv = { uvX1, 0.0f };
			v4.uv = { uvX2, 0.0f };

			vertices.push_back(v1);
			vertices.push_back(v2);
			vertices.push_back(v3);
			vertices.push_back(v4);
		}

		return vertices;
	}

	extern constexpr const std::vector<unsigned int> generatePrimitivePrismIndices(const unsigned int num_sides)
	{
		assert(num_sides > 2 && "Invalid number of sides");

		std::vector<unsigned int> indices;

		std::vector<unsigned int> indicesTop = generatePrimitiveFaceIndices(num_sides, 0, true);
		std::vector<unsigned int> indicesBottom = generatePrimitiveFaceIndices(num_sides, num_sides, false);

		indices.insert(indices.end(), indicesTop.begin(), indicesTop.end());
		indices.insert(indices.end(), indicesBottom.begin(), indicesBottom.end());

		const unsigned int from = num_sides * 2;
		const unsigned int to = num_sides * 6;
		for (unsigned int i = from; i < to; i += 4)
		{
			indices.push_back(i + 0);
			indices.push_back(i + 1);
			indices.push_back(i + 2);
			indices.push_back(i + 3);
			indices.push_back(i + 2);
			indices.push_back(i + 1);
		}

		return indices;
	}

	extern constexpr const std::vector<VertexComplete> generatePrimitiveSphereVertices(const unsigned int sub_divisions)
	{
		constexpr const float x = 0.525731112119133606f;
		constexpr const float z = 0.850650808352039932f;
		constexpr const float n = 0.f;

		constexpr const unsigned int vertice_count = 4 * 3;
		constexpr const Math::Vector3 vertices[vertice_count] =
		{
			{-x,n,z}, { x,n, z}, {-x, n,-z}, { x, n,-z},
			{ n,z,x}, { n,z,-x}, { n,-z, x}, { n,-z,-x},
			{ z,x,n}, {-z,x, n}, { z,-x, n}, {-z,-x, n}
		};

		assert(sub_divisions > 0 && "Invalid number of sub divisions");

		return std::vector<VertexComplete>{ vertices, vertices + vertice_count };
	}

	extern constexpr const std::vector<unsigned int> generatePrimitiveSphereIndices(const unsigned int sub_divisions)
	{
		constexpr const unsigned int triangle_count = 5 * 4 * 3;
		constexpr const unsigned int triangles[triangle_count] =
		{
			0,4,1,  0,9,4,  9,5,4,  4,5,8,  4,8,1,
			8,10,1, 8,3,10, 5,3,8,  5,2,3,  2,7,3,
			7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6,
			6,1,10, 9,0,11, 9,11,2, 9,2,5,  7,2,11
		};

		assert(sub_divisions > 0 && "Invalid number of sub divisions");

		return std::vector<unsigned int>{ triangles, triangles + triangle_count };
	}

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
