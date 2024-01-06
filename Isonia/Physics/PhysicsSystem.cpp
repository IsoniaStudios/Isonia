#include "PhysicsSystem.h"

#include "../Components/Gravity.h"
#include "../Components/RigidBody.h"
#include "../Components/Transform.h"
#include "../ECS/Coordinator.h"

extern Isonia::ECS::Coordinator gCoordinator;

namespace Isonia::Physics
{
	void PhysicsSystem::Init()
	{
	}

	void PhysicsSystem::Update(float dt)
	{
		for (auto const& entity : mEntities)
		{
			auto& rigidBody = gCoordinator.GetComponent<Components::RigidBody>(entity);
			auto& transform = gCoordinator.GetComponent<Components::Transform>(entity);

			// Forces
			auto const& gravity = gCoordinator.GetComponent<Components::Gravity>(entity);

			transform.position += rigidBody.velocity * dt;

			rigidBody.velocity += gravity.acceleration * dt;
		}
	}
}
