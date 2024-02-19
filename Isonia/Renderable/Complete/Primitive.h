#pragma once

// internal
#include "Vertex.h"
#include "Builder.h"
#include "../../Utilities/MathUtility.h"

// std
#include <stdexcept>

namespace Isonia::Renderable::Complete
{
	struct Primitive
	{
		enum Type {
			Quad,
			Cube,
			Icosahedron,
			HexagonalPrism,
		};

		static constexpr const Vertex* Vertices(const Type type)
		{
			switch (type)
			{
			case Type::Quad:
				return PrimitiveQuad::Vertices;
			case Type::Cube:
				return PrimitiveCube::Vertices;
			case Type::Icosahedron:
				return PrimitiveIcosahedron::Vertices;
			case Type::HexagonalPrism:
				return PrimitiveHexagonalPrism::Vertices;
			}
			throw std::invalid_argument("Unknown Primitive Type");
		};
		static constexpr const uint32_t VerticesCount(const Type type)
		{
			switch (type)
			{
			case Type::Quad:
				return PrimitiveQuad::VerticesCount;
			case Type::Cube:
				return PrimitiveCube::VerticesCount;
			case Type::Icosahedron:
				return PrimitiveIcosahedron::VerticesCount;
			case Type::HexagonalPrism:
				return PrimitiveHexagonalPrism::VerticesCount;
			}
			throw std::invalid_argument("Unknown Primitive Type");
		};

		static constexpr const uint32_t* Indices(const Type type)
		{
			switch (type)
			{
			case Type::Quad:
				return PrimitiveQuad::Indices;
			case Type::Cube:
				return PrimitiveCube::Indices;
			case Type::Icosahedron:
				return PrimitiveIcosahedron::Indices;
			case Type::HexagonalPrism:
				return PrimitiveHexagonalPrism::Indices;
			}
			throw std::invalid_argument("Unknown Primitive Type");
		};
		static constexpr const uint32_t IndicesCount(const Type type)
		{
			switch (type)
			{
			case Type::Quad:
				return PrimitiveQuad::IndicesCount;
			case Type::Cube:
				return PrimitiveCube::IndicesCount;
			case Type::Icosahedron:
				return PrimitiveIcosahedron::IndicesCount;
			case Type::HexagonalPrism:
				return PrimitiveHexagonalPrism::IndicesCount;
			}
			throw std::invalid_argument("Unknown Primitive Type");
		};

	private:
		struct PrimitiveQuad
		{
			static constexpr const Vertex Vertices[] = {
				{ { -0.5f, 0.0f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
				{ {  0.5f, 0.0f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
				{ { -0.5f, 0.0f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
				{ {  0.5f, 0.0f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } }
			};
			static constexpr const uint32_t VerticesCount = 4;

			static constexpr const uint32_t Indices[] = {
				2, 1, 0,
				1, 2, 3
			};
			static constexpr const uint32_t IndicesCount = 3 * 2;
		};

		struct PrimitiveCube
		{
			static constexpr const Vertex Vertices[] = {
				// front
				{ {-1.0f, +1.0f, +1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} },
				{ {+1.0f, +1.0f, +1.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },
				{ {-1.0f, -1.0f, +1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
				{ {+1.0f, -1.0f, +1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },
				// back
				{ {+1.0f, +1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },
				{ {-1.0f, +1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
				{ {+1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },
				{ {-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },
				// right
				{ {+1.0f, +1.0f, +1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
				{ {+1.0f, +1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
				{ {+1.0f, -1.0f, +1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
				{ {+1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
				// left
				{ {-1.0f, +1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
				{ {-1.0f, +1.0f, +1.0f}, {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
				{ {-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
				{ {-1.0f, -1.0f, +1.0f}, {0.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
				// top
				{ {-1.0f, -1.0f, +1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
				{ {+1.0f, -1.0f, +1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
				{ {-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },
				{ {+1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} },
				// bottom
				{ {-1.0f, +1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
				{ {+1.0f, +1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} },
				{ {-1.0f, +1.0f, +1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },
				{ {+1.0f, +1.0f, +1.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f} } 
			};
			static constexpr const uint32_t VerticesCount = 4 * 6;

			static constexpr const uint32_t Indices[] = {
				// front
				2 +  0, 1 +  0, 0 +  0,
				1 +  0, 2 +  0, 3 +  0,
				// back
				2 +  4, 1 +  4, 0 +  4,
				1 +  4, 2 +  4, 3 +  4,
				// right
				2 +  8, 1 +  8, 0 +  8,
				1 +  8, 2 +  8, 3 +  8,
				// left
				2 + 12, 1 + 12, 0 + 12,
				1 + 12, 2 + 12, 3 + 12,
				// top
				2 + 16, 1 + 16, 0 + 16,
				1 + 16, 2 + 16, 3 + 16,
				// bottom
				2 + 20, 1 + 20, 0 + 20,
				1 + 20, 2 + 20, 3 + 20,
			};
			static constexpr const uint32_t IndicesCount = 3 * 12;
		};

		struct PrimitiveIcosahedron
		{
		private:
			static constexpr const float s = 0.3f;
			static constexpr const float t = ((1.0f + 2.2360679775f) / 2.0f) * s; // (1 + sqrt(5)) / 2
		public:
			static constexpr const Vertex Vertices[] = {
				{ {    -s,     t,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.875f,  0.5f } },
				{ {     s,     t,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.625f, 0.75f } },
				{ {    -s,    -t,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.625f,  0.5f } },
				{ {     s,    -t,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.375f,  1.0f } },
				{ {  0.0f,    -s,     t }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.375f, 0.75f } },
				{ {  0.0f,     s,     t }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.625f,  0.0f } },
				{ {  0.0f,    -s,    -t }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.375f, 0.25f } },
				{ {  0.0f,     s,    -t }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.375f,  0.0f } },
				{ {     t, -0.0f,    -s }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.875f,  0.5f } },
				{ {     t, -0.0f,     s }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.625f, 0.75f } },
				{ {    -t, -0.0f,    -s }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.625f,  0.5f } },
				{ {    -t, -0.0f,     s }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.375f,  1.0f } }
			};
			static constexpr const uint32_t VerticesCount = 12;

			static constexpr const uint32_t Indices[] = {
				0, 11,  5,
				0,  5,  1,
				0,  1,  7,
				0,  7, 10,
				0, 10, 11,
				1,  5,  9,
				5, 11,  4,
			   11, 10,  2,
			   10,  7,  6,
				7,  1,  8,
				3,  9,  4,
				3,  4,  2,
				3,  2,  6,
				3,  6,  8,
				3,  8,  9,
				4,  9,  5,
				2,  4, 11,
				6,  2, 10,
				8,  6,  7,
				9,  8,  1
			};
			static constexpr const uint32_t IndicesCount = 3 * 20;
		};

		struct PrimitiveHexagonalPrism
		{
			static constexpr const Vertex Vertices[] = {
				// Top hexagon
				{ {0.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.5f, 1.0f} },
				{ {0.866f, 0.5f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.75f} },
				{ {0.866f, -0.5f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.25f} },
				{ {0.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.5f, 0.0f} },
				{ {-0.866f, -0.5f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.25f} },
				{ {-0.866f, 0.5f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.75f} },
				// Bottom hexagon
				{ {0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f} },
				{ {0.866f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.75f} },
				{ {0.866f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.25f} },
				{ {0.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.0f} },
				{ {-0.866f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.25f} },
				{ {-0.866f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.75f} }
			};
			static constexpr const uint32_t VerticesCount = 6 * 2;

			static constexpr const uint32_t Indices[] = {
				// Top face
				0, 1, 5,
				1, 2, 5,
				2, 3, 5,
				3, 4, 5,
				// Bottom face
				6, 7, 11,
				7, 8, 11,
				8, 9, 11,
				9, 10, 11,
				// Side faces
				0, 6, 1,
				1, 7, 2,
				2, 8, 3,
				3, 9, 4,
				4, 10, 5,
				5, 11, 0
			};
			static constexpr const uint32_t IndicesCount = 3 * 18;
		};
	};
}
