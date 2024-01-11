#pragma once

// internal
#include "ComponentArray.h"
#include "Definitions.h"

// std
#include <any>
#include <memory>
#include <unordered_map>

namespace Isonia::ECS
{
	class ComponentManager
	{
	public:
		template<typename T>
		void RegisterComponent()
		{
			const char* typeName = typeid(T).name();

			assert(componentTypes.find(typeName) == componentTypes.end() && "Registering component type more than once.");

			componentTypes.insert({ typeName, nextComponentType });
			componentArrays.insert({ typeName, new ComponentArray<T>() });
			++nextComponentType;
		}

		template<typename T>
		ComponentType GetComponentType()
		{
			const char* typeName = typeid(T).name();
			assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered before use.");
			return componentTypes[typeName];
		}

		template<typename T>
		void AddComponent(Entity entity, T component)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			componentArray->InsertData(entity, component);
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			componentArray->RemoveData(entity);
		}

		template<typename T>
		T* GetComponent(Entity entity)
		{
			ComponentArray<T>* componentArray = GetComponentArray<T>();
			return componentArray->GetData(entity);
		}

		void EntityDestroyed(Entity entity)
		{
			for (auto const& pair : componentArrays)
			{
				auto const& component = pair.second;
				component->EntityDestroyed(entity);
			}
		}

	private:
		std::unordered_map<const char*, ComponentType> componentTypes{};
		std::unordered_map<const char*, IComponentArray*> componentArrays{};
		ComponentType nextComponentType{};

		template<typename T>
		ComponentArray<T>* GetComponentArray()
		{
			const char* typeName = typeid(T).name();
			assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered before use.");
			IComponentArray* basePtr = componentArrays[typeName];
			return static_cast<ComponentArray<T>*>(basePtr);
		}
	};
}
