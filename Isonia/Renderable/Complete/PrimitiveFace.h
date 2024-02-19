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
		
		static constexpr const std::vector<uint32_t> GenerateFaceIndices(const uint32_t numSides, const uint32_t offset = 0, const bool faceUp = true)
		{
			assert(numSides > 2 && "Invalid number of sides");

			const uint32_t numTriangles = numSides - 2;

			std::vector<uint32_t> indices;
			for (uint32_t i = 0; i < numTriangles; i++)
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
	};
}
