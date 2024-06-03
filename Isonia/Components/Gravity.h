#pragma once

// internal
#include "../ECS/Definitions.h"
#include "../Math/Vector.h"

namespace Isonia::Components
{
	struct Gravity : ECS::Archetype<2>
	{
	public:
		Math::Vector3 acceleration{ 0.f,9.80665f,0.f };
	};
}
