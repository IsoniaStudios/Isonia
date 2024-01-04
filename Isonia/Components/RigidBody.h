#pragma once

#include <glm/glm.hpp>

namespace Isonia::Components
{
	struct Gravity
	{
	public:
		glm::vec3 velocity;
		glm::vec3 acceleration;
	};
}
