#pragma once

// internal
#include "ComponentManager.h"
#include "EntityManager.h"
#include "EventManager.h"
#include "SystemManager.h"
#include "Definitions.h"

// std
#include <memory>
#include <functional>

namespace Isonia::ECS
{
    class Coordinator
    {
    public:
        Coordinator()
        {
            entityManager = new EntityManager();
            componentManager = new ComponentManager();
            systemManager = new SystemManager();
            eventManager = new EventManager();
        }

        ~Coordinator()
        {
            delete eventManager;
            delete systemManager;
            delete componentManager;
            delete entityManager;
        }

        // Entity methods
        Entity CreateEntity()
        {
            return entityManager->CreateEntity();
        }

        void DestroyEntity(Entity entity)
        {
            entityManager->DestroyEntity(entity);
            componentManager->EntityDestroyed(entity);
            systemManager->EntityDestroyed(entity);
        }

        // Component methods
        template<typename T>
        void RegisterComponent()
        {
            componentManager->RegisterComponent<T>();
        }

        template<typename T>
        void AddComponent(Entity entity, T component)
        {
            componentManager->AddComponent<T>(entity, component);

            auto signature = entityManager->GetSignature(entity);
            signature.set(GetComponentType<T>(), true);
            entityManager->SetSignature(entity, signature);

            systemManager->EntitySignatureChanged(entity, signature);
        }

        template<typename T>
        void RemoveComponent(Entity entity)
        {
            componentManager->RemoveComponent<T>(entity);

            auto signature = entityManager->GetSignature(entity);
            signature.set(GetComponentType<T>(), false);
            entityManager->SetSignature(entity, signature);

            systemManager->EntitySignatureChanged(entity, signature);
        }

        template<typename T>
        T* GetComponent(Entity entity)
        {
            return componentManager->GetComponent<T>(entity);
        }

        // System methods
        template<typename T>
        T* RegisterSystem()
        {
            return systemManager->RegisterSystem<T>();
        }
        template<typename T>
        T* RegisterSystem(T* system)
        {
            return systemManager->RegisterSystem<T>(system);
        }

        template<typename T>
        void SetSystemSignature(Signature signature)
        {
            systemManager->SetSignature<T>(signature);
        }

        // Event methods
        void AddEventListener(EventId eventId, std::function<void(Event&)> const& listener)
        {
            eventManager->AddListener(eventId, listener);
        }

        void SendEvent(Event& event)
        {
            eventManager->SendEvent(event);
        }

        void SendEvent(EventId eventId)
        {
            eventManager->SendEvent(eventId);
        }

    private:
        EntityManager* entityManager;
        ComponentManager* componentManager;
        SystemManager* systemManager;
        EventManager* eventManager;
    };
}
