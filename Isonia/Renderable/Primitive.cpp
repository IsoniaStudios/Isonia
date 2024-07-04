// internal
#include "Renderable.h"

// external
#include <stdexcept>

namespace Isonia::Renderable
{
	extern constexpr void generatePrimitiveFaceVertices(VertexComplete* vertices, const unsigned int num_sides, const float height)
	{
		const float angle_increment = Math::two_pi / static_cast<float>(num_sides);
		const float angle_offset = num_sides % 2u == 0u ? angle_increment * 0.5f : 0.0f;
		const float radius = 0.5f;

		const Math::Vector3 normal = height == 0.0f ? Math::Vector3{ 0.0f, -1.0f, 0.0f } : Math::vec3Normalize(0.0f, height, 0.0f);
		const Math::Vector3 color = Math::Vector3{ 0.0f, 0.0f, 0.0f };

		const unsigned int num_vertices = generatePrimitiveFaceVerticesCount(num_sides);
		for (unsigned int i = 0u; i < num_vertices; i++)
		{
			const float z = radius * Math::cosf(static_cast<float>(i) * angle_increment + angle_offset);
			const float x = radius * Math::sinf(static_cast<float>(i) * angle_increment + angle_offset);
			const Math::Vector3 position{ x, height, z };
			const Math::Vector2 uv{ x + radius, z + radius };
			vertices[i] = VertexComplete{ position, position, normal, uv, };
		}
	}
	extern constexpr const VertexComplete* generatePrimitiveFaceVertices(const unsigned int num_sides, const float height)
	{
		const unsigned int num_vertices = generatePrimitiveFaceVerticesCount(num_sides);
		VertexComplete* vertices = new VertexComplete[num_vertices];
		generatePrimitiveFaceVertices(vertices, num_sides, height);
		return vertices;
	}
	extern constexpr const unsigned int generatePrimitiveFaceVerticesCount(const unsigned int num_sides)
	{
		return num_sides;
	}

	extern constexpr void generatePrimitiveFaceIndices(unsigned int* indices, const unsigned int num_sides, const unsigned int offset, const bool faceUp)
	{
		const unsigned int num_triangles = num_sides - 2u;
		for (unsigned int i = 0u; i < num_triangles; i++)
		{
			unsigned int index = i * 3u;
			indices[index + 0u] = 0u + offset;
			if (faceUp)
			{
				indices[index + 1u] = i + 2u + offset;
				indices[index + 2u] = i + 1u + offset;
			}
			else
			{
				indices[index + 1u] = i + 1u + offset;
				indices[index + 2u] = i + 2u + offset;
			}
		}
	}
	extern constexpr const unsigned int* generatePrimitiveFaceIndices(const unsigned int num_sides, const unsigned int offset, const bool faceUp)
	{
		const unsigned int num_indices = generatePrimitiveFaceIndicesCount(num_sides);
		unsigned int* indices = new unsigned int[num_indices];
		generatePrimitiveFaceIndices(indices, num_sides, offset, faceUp);
		return indices;
	}
	extern constexpr const unsigned int generatePrimitiveFaceIndicesCount(const unsigned int num_sides)
	{
		const unsigned int num_triangles = num_sides - 2u;
		return num_triangles * 3u;
	}

	extern constexpr void generatePrimitivePrismVertices(VertexComplete* vertices, const unsigned int num_sides)
	{
		VertexComplete* vertices_top = vertices;
		VertexComplete* vertices_bottom = vertices_top + generatePrimitiveFaceVerticesCount(num_sides);
		VertexComplete* vertices_side = vertices_bottom + generatePrimitiveFaceVerticesCount(num_sides);
		generatePrimitiveFaceVertices(vertices_top, num_sides, -0.5f);
		generatePrimitiveFaceVertices(vertices_bottom, num_sides, 0.5f);

		const float angleIncrement = Math::two_pi / num_sides;
		const float radius = 0.5f;

		for (unsigned int i = 0u; i < num_sides; i++)
		{
			const unsigned int next_i = (i + 1u) % num_sides;
			const unsigned int index = i * 4u;

			VertexComplete v1 = { vertices_top[i] };
			VertexComplete v2 = { vertices_top[next_i] };
			VertexComplete v3 = { vertices_bottom[i] };
			VertexComplete v4 = { vertices_bottom[next_i] };

			const Math::Vector3 normal = Math::vec3Normalize(
				(v1.position.x + v2.position.x) * 0.5f,
				0.0f,
				(v1.position.z + v2.position.z) * 0.5f
			);
			v1.normal = normal;
			v2.normal = normal;
			v3.normal = normal;
			v4.normal = normal;

			const float uvX1 = static_cast<float>(i + 0u) / static_cast<float>(num_sides);
			const float uvX2 = static_cast<float>(i + 1u) / static_cast<float>(num_sides);
			v1.uv = { uvX1, 1.0f };
			v2.uv = { uvX2, 1.0f };
			v3.uv = { uvX1, 0.0f };
			v4.uv = { uvX2, 0.0f };

			vertices_side[index + 0u] = v1;
			vertices_side[index + 1u] = v2;
			vertices_side[index + 2u] = v3;
			vertices_side[index + 3u] = v4;
		}
	}
	extern constexpr const VertexComplete* generatePrimitivePrismVertices(const unsigned int num_sides)
	{
		const unsigned int num_vertices = generatePrimitivePrismVerticesCount(num_sides);
		VertexComplete* vertices = new VertexComplete[num_vertices];
		generatePrimitivePrismVertices(vertices, num_sides);
		return vertices;
	}
	extern constexpr const unsigned int generatePrimitivePrismVerticesCount(const unsigned int num_sides)
	{
		return generatePrimitiveFaceVerticesCount(num_sides) * 2u + num_sides * 4u;
	}

	extern constexpr void generatePrimitivePrismIndices(unsigned int* indices, const unsigned int num_sides)
	{
		unsigned int* indices_top = indices;
		unsigned int* indices_bottom = indices_top + generatePrimitiveFaceIndicesCount(num_sides);
		unsigned int* indices_side = indices_bottom + generatePrimitiveFaceIndicesCount(num_sides);
		generatePrimitiveFaceIndices(indices_top, num_sides, 0u, true);
		generatePrimitiveFaceIndices(indices_bottom, num_sides, num_sides, true);

		unsigned int index = 0u;
		const unsigned int to = num_sides * 4u;
		for (unsigned int i = 0u; i < to; i += 4u)
		{
			indices_side[index++] = i + 0;
			indices_side[index++] = i + 1;
			indices_side[index++] = i + 2;
			indices_side[index++] = i + 3;
			indices_side[index++] = i + 2;
			indices_side[index++] = i + 1;
		}
	}
	extern constexpr const unsigned int* generatePrimitivePrismIndices(const unsigned int num_sides)
	{
		const unsigned int num_indices = generatePrimitivePrismIndicesCount(num_sides);
		unsigned int* indices = new unsigned int[num_indices];
		generatePrimitivePrismIndices(indices, num_sides);
		return indices;
	}
	extern constexpr const unsigned int generatePrimitivePrismIndicesCount(const unsigned int num_sides)
	{
		return generatePrimitiveFaceIndicesCount(num_sides) * 2u + num_sides * 4;
	}

	extern constexpr void generatePrimitiveSphereVertices(VertexComplete* vertices, const unsigned int sub_divisions)
	{
		constexpr const float x = 0.525731112119133606f;
		constexpr const float z = 0.850650808352039932f;
		constexpr const float n = 0.f;

		vertices = new VertexComplete[]
		{
			{-x,n,z}, { x,n, z}, {-x, n,-z}, { x, n,-z},
			{ n,z,x}, { n,z,-x}, { n,-z, x}, { n,-z,-x},
			{ z,x,n}, {-z,x, n}, { z,-x, n}, {-z,-x, n}
		};
	}
	extern constexpr const VertexComplete* generatePrimitiveSphereVertices(const unsigned int sub_divisions)
	{
		const unsigned int num_vertices = generatePrimitiveSphereVerticesCount(sub_divisions);
		VertexComplete* vertices = new VertexComplete[num_vertices];
		generatePrimitiveSphereVertices(vertices, sub_divisions);
		return vertices;
	}
	extern constexpr const unsigned int generatePrimitiveSphereVerticesCount(const unsigned int sub_divisions)
	{
		return 4u * 3u;
	}

	extern constexpr void generatePrimitiveSphereIndices(unsigned int* indices, const unsigned int sub_divisions)
	{
		indices = new unsigned int[]
		{
			0,4,1,  0,9,4,  9,5,4,  4,5,8,  4,8,1,
			8,10,1, 8,3,10, 5,3,8,  5,2,3,  2,7,3,
			7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6,
			6,1,10, 9,0,11, 9,11,2, 9,2,5,  7,2,11
		};
	}
	extern constexpr const unsigned int* generatePrimitiveSphereIndices(const unsigned int sub_divisions)
	{
		const unsigned int num_indices = generatePrimitiveSphereVerticesCount(sub_divisions);
		unsigned int* indices = new unsigned int[num_indices];
		generatePrimitiveSphereIndices(indices, sub_divisions);
		return indices;
	}
	extern constexpr const unsigned int generatePrimitiveSphereIndicesCount(const unsigned int sub_divisions)
	{
		return 5u * 4u * 3u;
	}

	extern constexpr void generatePrimitiveVertices(VertexComplete* vertices, const PrimitiveType type)
	{
		switch (type)
		{
		case PrimitiveType::Triangle:
			generatePrimitiveFaceVertices(vertices, 3);
			break;
		case PrimitiveType::Quad:
			generatePrimitiveFaceVertices(vertices, 4);
			break;
		case PrimitiveType::Pentagon:
			generatePrimitiveFaceVertices(vertices, 5);
			break;
		case PrimitiveType::Hexagon:
			generatePrimitiveFaceVertices(vertices, 6);
			break;

		case PrimitiveType::TriangularPrism:
			generatePrimitivePrismVertices(vertices, 3);
			break;
		case PrimitiveType::Cube:
			generatePrimitivePrismVertices(vertices, 4);
			break;
		case PrimitiveType::PentagonalPrism:
			generatePrimitivePrismVertices(vertices, 5);
			break;
		case PrimitiveType::HexagonalPrism:
			generatePrimitivePrismVertices(vertices, 6);
			break;

		case PrimitiveType::Icosahedron:
			generatePrimitiveSphereVertices(vertices, 1);
			break;

		default:
			throw std::invalid_argument("Unknown Primitive Type");
		}
	};
	extern constexpr const VertexComplete* generatePrimitiveVertices(const PrimitiveType type)
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

		default:
			throw std::invalid_argument("Unknown Primitive Type");
		}
	};
	extern constexpr const unsigned int generatePrimitiveVerticesCount(const PrimitiveType type)
	{
		switch (type)
		{
		case PrimitiveType::Triangle:
			return generatePrimitiveFaceVerticesCount(3);
		case PrimitiveType::Quad:
			return generatePrimitiveFaceVerticesCount(4);
		case PrimitiveType::Pentagon:
			return generatePrimitiveFaceVerticesCount(5);
		case PrimitiveType::Hexagon:
			return generatePrimitiveFaceVerticesCount(6);

		case PrimitiveType::TriangularPrism:
			return generatePrimitivePrismVerticesCount(3);
		case PrimitiveType::Cube:
			return generatePrimitivePrismVerticesCount(4);
		case PrimitiveType::PentagonalPrism:
			return generatePrimitivePrismVerticesCount(5);
		case PrimitiveType::HexagonalPrism:
			return generatePrimitivePrismVerticesCount(6);

		case PrimitiveType::Icosahedron:
			return generatePrimitiveSphereVerticesCount(1);

		default:
			throw std::invalid_argument("Unknown Primitive Type");
		}
	};

	extern constexpr void generatePrimitiveIndices(unsigned int* indices, const PrimitiveType type)
	{
		switch (type)
		{
		case PrimitiveType::Triangle:
			return generatePrimitiveFaceIndices(indices, 3);
		case PrimitiveType::Quad:
			return generatePrimitiveFaceIndices(indices, 4);
		case PrimitiveType::Pentagon:
			return generatePrimitiveFaceIndices(indices, 5);
		case PrimitiveType::Hexagon:
			return generatePrimitiveFaceIndices(indices, 6);

		case PrimitiveType::TriangularPrism:
			return generatePrimitivePrismIndices(indices, 3);
		case PrimitiveType::Cube:
			return generatePrimitivePrismIndices(indices, 4);
		case PrimitiveType::PentagonalPrism:
			return generatePrimitivePrismIndices(indices, 5);
		case PrimitiveType::HexagonalPrism:
			return generatePrimitivePrismIndices(indices, 6);

		case PrimitiveType::Icosahedron:
			return generatePrimitiveSphereIndices(indices, 1);

		default:
			throw std::invalid_argument("Unknown Primitive Type");
		}
	};
	extern constexpr const unsigned int* generatePrimitiveIndices(const PrimitiveType type)
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

		default:
			throw std::invalid_argument("Unknown Primitive Type");
		}
	};
	extern constexpr const unsigned int generatePrimitiveIndicesCount(const PrimitiveType type)
	{
		switch (type)
		{
		case PrimitiveType::Triangle:
			return generatePrimitiveFaceIndicesCount(3);
		case PrimitiveType::Quad:
			return generatePrimitiveFaceIndicesCount(4);
		case PrimitiveType::Pentagon:
			return generatePrimitiveFaceIndicesCount(5);
		case PrimitiveType::Hexagon:
			return generatePrimitiveFaceIndicesCount(6);

		case PrimitiveType::TriangularPrism:
			return generatePrimitivePrismIndicesCount(3);
		case PrimitiveType::Cube:
			return generatePrimitivePrismIndicesCount(4);
		case PrimitiveType::PentagonalPrism:
			return generatePrimitivePrismIndicesCount(5);
		case PrimitiveType::HexagonalPrism:
			return generatePrimitivePrismIndicesCount(6);

		case PrimitiveType::Icosahedron:
			return generatePrimitiveSphereIndicesCount(1);

		default:
			throw std::invalid_argument("Unknown Primitive Type");
		}
	};
}
