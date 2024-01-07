#pragma once

#include <glm/glm.hpp>

namespace Isonia::Components
{
	struct Gravity
	{
	public:
		glm::vec3 acceleration{ 0.f,9.80665f,0.f };
	};
}
