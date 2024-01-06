#pragma once

#include <glm/glm.hpp>

namespace Isonia::Components
{
	struct Transform
	{
	public:
		glm::vec3 position{};
		glm::vec3 rotation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 Mat4();

		glm::mat3 NormalMatrix();
	};
}
