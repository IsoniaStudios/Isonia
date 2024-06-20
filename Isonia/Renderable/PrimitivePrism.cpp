// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	static constexpr const std::vector<VertexComplete> generatePrimitivePrismVertices(const unsigned int num_sides)
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

	static constexpr const std::vector<unsigned int> generatePrimitivePrismIndices(const unsigned int num_sides)
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
}
