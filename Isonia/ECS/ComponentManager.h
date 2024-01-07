#pragma once

#include "ComponentArray.h"
#include "Definitions.h"
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

			assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");

			mComponentTypes.insert({ typeName, mNextComponentType });
			mComponentArrays.insert({ typeName, new ComponentArray<T>() });
			++mNextComponentType;
		}

		template<typename T>
		ComponentType GetComponentType()
		{
			const char* typeName = typeid(T).name();
			assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");
			return mComponentTypes[typeName];
		}

		template<typename T>
		void AddComponent(Entity entity, T component)
		{
			GetComponentArray<T>()->InsertData(entity, component);
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			GetComponentArray<T>()->RemoveData(entity);
		}

		template<typename T>
		T* GetComponent(Entity entity)
		{
			return GetComponentArray<T>()->GetData(entity);
		}

		void EntityDestroyed(Entity entity);

	private:
		std::unordered_map<const char*, ComponentType> mComponentTypes{};
		std::unordered_map<const char*, IComponentArray*> mComponentArrays{};
		ComponentType mNextComponentType{};

		template<typename T>
		ComponentArray<T>* GetComponentArray()
		{
			const char* typeName = typeid(T).name();
			assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");
			IComponentArray* basePtr = mComponentArrays[typeName];
			return static_cast<ComponentArray<T>*>(basePtr);
		}
	};
}
