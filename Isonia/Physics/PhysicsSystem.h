#pragma once

#include "../ECS/System.h"

namespace Isonia::Physics
{
	class PhysicsSystem : public ECS::System
	{
	public:
		void Init();

		void Update(float dt);
	};
}
