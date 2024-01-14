#pragma once

// internal
#include "../ECS/Definitions.h"

// glm
#include <glm/glm.hpp>

namespace Isonia::Components
{
	struct Gravity : ECS::Archetype<2>
	{
	public:
		glm::vec3 acceleration{ 0.f,9.80665f,0.f };
	};
}
