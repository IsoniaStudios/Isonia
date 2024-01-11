#pragma once

// glm
#include <glm/glm.hpp>

namespace Isonia::Components
{
	struct RigidBody
	{
	public:
		RigidBody() : velocity(0.f, 0.f, 0.f), acceleration(0.f, 0.f, 0.f)
		{
		}

		RigidBody(glm::vec3 initialVelocity, glm::vec3 initialAcceleration)
			: velocity(initialVelocity), acceleration(initialAcceleration)
		{
		}

		glm::vec3 velocity;
		glm::vec3 acceleration;
	};
}