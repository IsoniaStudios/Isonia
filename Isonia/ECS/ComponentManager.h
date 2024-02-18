#pragma once

// internal
#include "ComponentArray.h"
#include "Definitions.h"

// std
#include <any>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <typeindex>

namespace Isonia::ECS
{
	class ComponentManager
	{
    public:
        ~ComponentManager()
        {
            for (ComponentType i = 0; i < MAX_COMPONENTS; ++i)
            {
                delete componentArrays[i];
            }
            //delete[] componentArrays;
        }

        template<typename T>
        void RegisterComponent()
        {
            constexpr ComponentType type = GetComponentType<T>();
            static_assert(type < MAX_COMPONENTS, "Exceeded maximum component types.");
            componentArrays[type] = new ComponentArray<T>();
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
            for (auto& component : componentArrays)
            {
                if (component)
                {
                    component->EntityDestroyed(entity);
                }
            }
        }

    private:
        IComponentArray* componentArrays[MAX_COMPONENTS];

        template<typename T>
        ComponentArray<T>* GetComponentArray()
        {
            constexpr ComponentType type = GetComponentType<T>();
            static_assert(type < MAX_COMPONENTS && "Invalid component type.");
            return reinterpret_cast<ComponentArray<T>*>(componentArrays[type]);
        }
	};
}
