#pragma once

// internal
#include "Definitions.h"

// std
#include <set>

namespace Isonia::ECS
{
	class System
	{
	public:
		std::set<Entity> entities;
	};
}
