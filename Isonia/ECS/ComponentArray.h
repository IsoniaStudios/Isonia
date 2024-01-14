#pragma once

// internal
#include "IComponentArray.h"

// std
#include <array>
#include <cassert>
#include <unordered_map>

namespace Isonia::ECS
{
	template<typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		ComponentArray()
		{
			//if constexpr (UtilizesArchetype<T>())
			//{
			//	entityToIndexMap.reserve(MAX_ENTITIES);
			//	indexToEntityMap.reserve(MAX_ENTITIES);
			//}
		}

		void InsertData(Entity entity, T component)
		{
			assert(entityToIndexMap.find(entity) == entityToIndexMap.end() && "Component added to same entity more than once.");

			// Put new entry at end
			Entity newIndex = size;
			entityToIndexMap.emplace(entity, newIndex);
			indexToEntityMap.emplace(newIndex, entity);
			componentArray[newIndex] = component;
			++size;
		}

		void RemoveData(Entity entity)
		{
			assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Removing non-existent component.");

			// Copy element at end into deleted element's place to maintain density
			Entity indexOfRemovedEntity = entityToIndexMap.find(entity)->second;
			Entity indexOfLastElement = size - 1;
			componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement];

			// Update map to point to moved spot
			Entity entityOfLastElement = indexToEntityMap.find(indexOfLastElement)->second;
			entityToIndexMap.emplace(entityOfLastElement, indexOfRemovedEntity);
			indexToEntityMap.emplace(indexOfRemovedEntity, entityOfLastElement);

			entityToIndexMap.erase(entity);
			indexToEntityMap.erase(indexOfLastElement);

			--size;
		}

		T* GetData(Entity entity)
		{
			assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Retrieving non-existent component.");
			Entity index = entityToIndexMap.find(entity)->second;
			return &componentArray[index];
		}

		void EntityDestroyed(Entity entity) override
		{
			if (entityToIndexMap.find(entity) != entityToIndexMap.end())
			{
				RemoveData(entity);
			}
		}

	private:
		T componentArray[MAX_ENTITIES]{};
		std::unordered_map<Entity, Entity> entityToIndexMap{};
		std::unordered_map<Entity, Entity> indexToEntityMap{};
		Entity size{};
	};
}
