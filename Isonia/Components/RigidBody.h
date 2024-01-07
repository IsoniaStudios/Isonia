#pragma once

#include <glm/glm.hpp>

namespace Isonia::Components
{
	struct RigidBody
	{
	public:
		glm::vec3 velocity{ 0.f, 0.f, 0.f };
		glm::vec3 acceleration{ 0.f, 0.f, 0.f };
	};
}
