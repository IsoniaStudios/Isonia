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
		void InsertData(Entity entity, T component)
		{
			assert(entityToIndexMap.find(entity) == entityToIndexMap.end() && "Component added to same entity more than once.");

			// Put new entry at end
			size_t newIndex = size;
			entityToIndexMap[entity] = newIndex;
			indexToEntityMap[newIndex] = entity;
			componentArray[newIndex] = component;
			++size;
		}

		void RemoveData(Entity entity)
		{
			assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Removing non-existent component.");

			// Copy element at end into deleted element's place to maintain density
			size_t indexOfRemovedEntity = entityToIndexMap[entity];
			size_t indexOfLastElement = size - 1;
			componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement];

			// Update map to point to moved spot
			Entity entityOfLastElement = indexToEntityMap[indexOfLastElement];
			entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
			indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

			entityToIndexMap.erase(entity);
			indexToEntityMap.erase(indexOfLastElement);

			--size;
		}

		T* GetData(Entity entity)
		{
			assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Retrieving non-existent component.");
			size_t index = entityToIndexMap[entity];
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
		std::unordered_map<Entity, size_t> entityToIndexMap{};
		std::unordered_map<size_t, Entity> indexToEntityMap{};
		size_t size{};
	};
}
