#pragma once

#include "Definitions.h"
#include <set>

namespace Isonia::ECS
{
	class System
	{
	public:
		std::set<Entity> mEntities;
	};
}
