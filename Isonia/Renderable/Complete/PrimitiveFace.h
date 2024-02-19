#pragma once

// internal
#include "Vertex.h"
#include "Builder.h"
#include "../../Utilities/MathUtility.h"

// std
#include <stdexcept>

namespace Isonia::Renderable::Complete
{
	struct PrimitiveFace
	{
		static constexpr const std::vector<Vertex> GenerateFaceVertices(const uint32_t numSides, const float height = 0.0f)
		{
			assert(numSides > 2 && "Invalid number of sides");

			const float angleIncrement = 2.0f * Utilities::Math::PI / static_cast<float>(numSides);
			const float radius = 0.5f;

			const glm::vec3 normal = height == 0.0f ? glm::vec3{ 0.0f, -1.0f, 0.0f } : glm::normalize(glm::vec3{ 0.0f, height, 0.0f });
			const glm::vec3 color = glm::vec3{ 0.0f, 0.0f, 0.0f };

			std::vector<Vertex> vertices;
			const float angleOffset = numSides % 2 == 0 ? angleIncrement * 0.5f : 0.0f;
			for (uint32_t i = 0; i < numSides; i++)
			{
				const float z = radius * cos(static_cast<float>(i) * angleIncrement + angleOffset);
				const float x = radius * sin(static_cast<float>(i) * angleIncrement + angleOffset);
				const glm::vec3 position{ x, height, z };
				const glm::vec2 uv{ x + radius, z + radius };
				vertices.push_back({ position, position, normal, uv, });
			}

			return vertices;
		}
		
		static constexpr const std::vector<uint32_t> GenerateFaceIndices(const uint32_t numSides)
		{
			// 3 -> 3 (3 * 1)
			// 4 -> 6 (3 * 2)
			// 5 -> 9 (3 * 3)
			// 6 -> 12 (3 * 4)
			// 6 -> 15 (3 * 5)

			assert(numSides > 2 && "Invalid number of sides");

			const uint32_t numTriangles = numSides - 2;
			const uint32_t numOuterTriangles = (numSides) / 2;
			// 3, 4, 5, 6, 7, 8, 9, 10, 11
			// 1, 2, 2, 3, 3, 4, 4,  5,  5

			std::vector<uint32_t> indices;

			// for every outer triangle
			indices.push_back(0);
			indices.push_back(2);
			indices.push_back(1);
			for (int32_t i = 1; i < numOuterTriangles; i++)
			{
				int32_t index = i * 3 - i;

				indices.push_back(index + 0);
				indices.push_back((index + 2) % numSides);
				indices.push_back(index + 1);
			}

			return indices;
		}
	};
}
