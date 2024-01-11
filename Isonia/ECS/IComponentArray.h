#pragma once

// internal
#include "Definitions.h"

namespace Isonia::ECS
{
	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;
		virtual void EntityDestroyed(Entity entity) = 0;
	};
}
