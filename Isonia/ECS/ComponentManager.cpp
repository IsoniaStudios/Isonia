#include "ComponentManager.h"

namespace Isonia::ECS
{
	void ComponentManager::EntityDestroyed(Entity entity)
	{
		for (auto const& pair : mComponentArrays)
		{
			auto const& component = pair.second;
			component->EntityDestroyed(entity);
		}
	}
}
