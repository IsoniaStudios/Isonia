#pragma once

// internal
#include "Vertex.h"
#include "Builder.h"

// std
#include <stdexcept>

namespace Isonia::Renderable::Complete
{
	struct PrimitiveQuad
	{
		static constexpr const Vertex* Vertices()
		{
			constexpr const Vertex vertices[] = {
				{ glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
				{ glm::vec3(0.5f, 0.0f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
				{ glm::vec3(-0.5f, 0.0f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
				{ glm::vec3(0.5f, 0.0f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)}
			};
			return vertices;
		};
		static constexpr const uint32_t VerticesCount()
		{
			return 4;
		};

		static constexpr const uint32_t* Indices()
		{
			constexpr const uint32_t indices[] = {
				0, 2, 1,
				2, 3, 1
			};
			return indices;
		};
		static constexpr const uint32_t IndicesCount()
		{
			return 6;
		};
	};

	struct PrimitiveCube
	{
		static constexpr const Vertex* Vertices()
		{
			constexpr const Vertex vertices[] = {
				{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.875f, 0.5f) },
				{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.625f, 0.75f) },
				{ glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.625f, 0.5f) },
				{ glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.375f, 1.0f) },
				{ glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.375f, 0.75f) },
				{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.625f, 0.0f) },
				{ glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.375f, 0.25f) },
				{ glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.375f, 0.0f) }
			};
			return vertices;
		};
		static constexpr const uint32_t VerticesCount()
		{
			return 8;
		};

		static constexpr const uint32_t* Indices()
		{
			constexpr const uint32_t indices[] = {
				0, 2, 1, //face front
				0, 3, 2,
				2, 3, 4, //face top
				2, 4, 5,
				1, 2, 5, //face right
				1, 5, 6,
				0, 7, 4, //face left
				0, 4, 3,
				5, 4, 7, //face back
				5, 7, 6,
				0, 6, 7, //face bottom
				0, 1, 6
			};
			return indices;
		};
		static constexpr const uint32_t IndicesCount()
		{
			return 36;
		};
	};

	struct Primitive
	{
		enum Type {
			Quad,
			Cube,
		};

		static constexpr const Vertex* Vertices(const Type type)
		{
			switch (type)
			{
			case Type::Quad:
				return PrimitiveQuad::Vertices();
			case Type::Cube:
				return PrimitiveCube::Vertices();
			}
			throw std::invalid_argument("Unknown Primitive Type");
		};
		static constexpr const uint32_t VerticesCount(const Type type)
		{
			switch (type)
			{
			case Type::Quad:
				return PrimitiveQuad::VerticesCount();
			case Type::Cube:
				return PrimitiveCube::VerticesCount();
			}
			throw std::invalid_argument("Unknown Primitive Type");
		};

		static constexpr const uint32_t* Indices(const Type type)
		{
			switch (type)
			{
			case Type::Quad:
				return PrimitiveQuad::Indices();
			case Type::Cube:
				return PrimitiveCube::Indices();
			}
			throw std::invalid_argument("Unknown Primitive Type");
		};
		static constexpr const uint32_t IndicesCount(const Type type)
		{
			switch (type)
			{
			case Type::Quad:
				return PrimitiveQuad::IndicesCount();
			case Type::Cube:
				return PrimitiveCube::IndicesCount();
			}
			throw std::invalid_argument("Unknown Primitive Type");
		};
	};
}
