#pragma once

// internal
#include "Vertex.h"
#include "Builder.h"
#include "PrimitiveFace.h"
#include "../../Utilities/MathUtility.h"

// std
#include <stdexcept>

namespace Isonia::Renderable::Complete
{
	struct PrimitivePrism : PrimitiveFace
	{
		static constexpr const std::vector<glm::vec3> GeneratePrismVertices(const uint32_t numSides)
		{
			assert(numSides > 2 && "Invalid number of sides");

			std::vector<glm::vec3> vertices;
			const float angleIncrement = 2.0f * Utilities::Math::PI / numSides;
			const float radius = 0.5f;

			// Generate vertices for the bottom face
			std::vector<Vertex> verticesBottom = GenerateFaceVertices(numSides, 0.5f);

			// Generate vertices for the top face
			std::vector<Vertex> verticesTop = GenerateFaceVertices(numSides, -0.5f);

			// Add vertices for the side faces
			for (uint32_t i = 0; i < numSides; i++)
			{
				int nextIndex = (i + 1) % numSides;
				vertices.push_back(glm::vec3(vertices[i].x, vertices[i].y, vertices[i].z));
				vertices.push_back(glm::vec3(vertices[i + numSides].x, vertices[i + numSides].y, vertices[i + numSides].z));
				vertices.push_back(glm::vec3(vertices[nextIndex].x, vertices[nextIndex].y, vertices[nextIndex].z));

				vertices.push_back(glm::vec3(vertices[nextIndex].x, vertices[nextIndex].y, vertices[nextIndex].z));
				vertices.push_back(glm::vec3(vertices[nextIndex + numSides].x, vertices[nextIndex + numSides].y, vertices[nextIndex + numSides].z));
				vertices.push_back(glm::vec3(vertices[i + numSides].x, vertices[i + numSides].y, vertices[i + numSides].z));
			}

			return vertices;
		}
	};
}
