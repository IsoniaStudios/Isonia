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
}
