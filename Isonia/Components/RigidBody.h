#pragma once

#include <glm/glm.hpp>

namespace Isonia::Components
{
	struct RigidBody
	{
	public:
		glm::vec3 velocity;
		glm::vec3 acceleration;
	};
}
