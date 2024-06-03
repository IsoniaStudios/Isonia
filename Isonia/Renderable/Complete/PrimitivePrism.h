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
	struct PrimitivePrism : protected PrimitiveFace
	{
		static constexpr const std::vector<Vertex> GeneratePrismVertices(const uint32_t numSides)
		{
			assert(numSides > 2 && "Invalid number of sides");

			std::vector<Vertex> vertices;
			const float angleIncrement = Math::TwoPI / numSides;
			const float radius = 0.5f;

			std::vector<Vertex> verticesTop = GenerateFaceVertices(numSides, -0.5f);
			std::vector<Vertex> verticesBottom = GenerateFaceVertices(numSides, 0.5f);

			vertices.insert(vertices.end(), verticesTop.begin(), verticesTop.end());
			vertices.insert(vertices.end(), verticesBottom.begin(), verticesBottom.end());

			// Add vertices for the side faces
			for (uint32_t i = 0; i < numSides; i++)
			{
				const uint32_t nextIndex = (i + 1) % numSides;
				
				Vertex v1 = { verticesTop[i] };
				Vertex v2 = { verticesTop[nextIndex] };
				Vertex v3 = { verticesBottom[i] };
				Vertex v4 = { verticesBottom[nextIndex] };

				const Math::Vector3 normal = Math::Normalize(Math::Vector3{
					(v1.position.x + v2.position.x) * 0.5f,
					0.0f,
					(v1.position.z + v2.position.z) * 0.5f,
				});
				v1.normal = normal;
				v2.normal = normal;
				v3.normal = normal;
				v4.normal = normal;

				const float uvX1 = static_cast<float>(i) / static_cast<float>(numSides);
				const float uvX2 = static_cast<float>(i + 1) / static_cast<float>(numSides);
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

		static constexpr const std::vector<uint32_t> GeneratePrismIndices(const uint32_t numSides)
		{
			assert(numSides > 2 && "Invalid number of sides");

			std::vector<uint32_t> indices;

			std::vector<uint32_t> indicesTop = GenerateFaceIndices(numSides, 0, true);
			std::vector<uint32_t> indicesBottom = GenerateFaceIndices(numSides, numSides, false);

			indices.insert(indices.end(), indicesTop.begin(), indicesTop.end());
			indices.insert(indices.end(), indicesBottom.begin(), indicesBottom.end());

			const uint32_t from = numSides * 2;
			const uint32_t to = numSides * 6;
			for (uint32_t i = from; i < to; i += 4)
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
	};
}
