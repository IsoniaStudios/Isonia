#pragma once

// internal
#include "../Components/Gravity.h"
#include "../Components/RigidBody.h"
#include "../Components/Transform.h"

#include "../ECS/Coordinator.h"
#include "../ECS/System.h"

// global reference
extern Isonia::ECS::Coordinator gCoordinator;

namespace Isonia::Physics
{
	class PhysicsSystem : public ECS::System
	{
	public:
		void Init()
		{
		}

		void Update(float dt)
		{
			for (auto const& entity : mEntities)
			{
				auto* rigidBody = gCoordinator.GetComponent<Components::RigidBody>(entity);
				auto* transform = gCoordinator.GetComponent<Components::Transform>(entity);
				auto* const gravity = gCoordinator.GetComponent<Components::Gravity>(entity);

				transform->position += rigidBody->velocity * dt;
				rigidBody->velocity += gravity->acceleration * dt;
			}
		}
	};
}
