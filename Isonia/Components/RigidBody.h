#pragma once

// internal
#include "../ECS/Definitions.h"
#include "../Math/Vector.h"

namespace Isonia::Components
{
	struct RigidBody : ECS::Archetype<1>
	{
	public:
		RigidBody() : velocity(0.f, 0.f, 0.f), acceleration(0.f, 0.f, 0.f)
		{
		}

		RigidBody(Math::Vector3 initialVelocity, Math::Vector3 initialAcceleration)
			: velocity(initialVelocity), acceleration(initialAcceleration)
		{
		}

		Math::Vector3 velocity;
		Math::Vector3 acceleration;
	};
}
