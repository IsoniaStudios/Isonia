#pragma once

// std
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

	template <ComponentType componentType>
	struct Archetype
	{
		static constexpr ComponentType COMPONENT_TYPE = componentType;
	};

	template <ComponentType componentType>
	struct Component
	{
		static constexpr ComponentType COMPONENT_TYPE = componentType;
	};

	template <typename T>
	inline constexpr bool UtilizesArchetype() {
		return std::is_base_of<Archetype, T>::value;
	}

	template <typename T>
	inline constexpr ComponentType GetComponentType() {
		return T::COMPONENT_TYPE;
	}
}
