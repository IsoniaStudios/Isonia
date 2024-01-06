#pragma once

#include "IComponentArray.h"
#include <array>
#include <cassert>
#include <unordered_map>

namespace Isonia::ECS
{
	template<typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		void InsertData(Entity entity, T component);
		void RemoveData(Entity entity);

		T& GetData(Entity entity);

		void EntityDestroyed(Entity entity) override;

	private:
		std::array<T, MAX_ENTITIES> mComponentArray{};
		std::unordered_map<Entity, size_t> mEntityToIndexMap{};
		std::unordered_map<size_t, Entity> mIndexToEntityMap{};
		size_t mSize{};
	};
}
