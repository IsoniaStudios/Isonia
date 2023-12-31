#pragma once

#include <bitset>

namespace Isonia::ECS
{
	using Entity = std::uint16_t;
	const Entity MAX_ENTITIES = 5000;
	using ComponentType = std::uint8_t;
	const ComponentType MAX_COMPONENTS = 32;
	using Signature = std::bitset<MAX_COMPONENTS>;

	using EventId = std::uint16_t;
	using ParamId = std::uint16_t;
}
