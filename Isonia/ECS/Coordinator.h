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
        void Init()
        {
            mComponentManager = new ComponentManager();
            mEntityManager = new EntityManager();
            mEventManager = new EventManager();
            mSystemManager = new SystemManager();
        }

        // Entity methods
        Entity CreateEntity()
        {
            return mEntityManager->CreateEntity();
        }

        void DestroyEntity(Entity entity)
        {
            mEntityManager->DestroyEntity(entity);
            mComponentManager->EntityDestroyed(entity);
            mSystemManager->EntityDestroyed(entity);
        }

        // Component methods
        template<typename T>
        void RegisterComponent()
        {
            mComponentManager->RegisterComponent<T>();
        }

        template<typename T>
        void AddComponent(Entity entity, T component)
        {
            mComponentManager->AddComponent<T>(entity, component);

            auto signature = mEntityManager->GetSignature(entity);
            signature.set(mComponentManager->GetComponentType<T>(), true);
            mEntityManager->SetSignature(entity, signature);

            mSystemManager->EntitySignatureChanged(entity, signature);
        }

        template<typename T>
        void RemoveComponent(Entity entity)
        {
            mComponentManager->RemoveComponent<T>(entity);

            auto signature = mEntityManager->GetSignature(entity);
            signature.set(mComponentManager->GetComponentType<T>(), false);
            mEntityManager->SetSignature(entity, signature);

            mSystemManager->EntitySignatureChanged(entity, signature);
        }

        template<typename T>
        T* GetComponent(Entity entity)
        {
            return mComponentManager->GetComponent<T>(entity);
        }

        template<typename T>
        ComponentType GetComponentType()
        {
            return mComponentManager->GetComponentType<T>();
        }

        // System methods
        template<typename T>
        T* RegisterSystem()
        {
            return mSystemManager->RegisterSystem<T>();
        }
        template<typename T>
        T* RegisterSystem(T* system)
        {
            return mSystemManager->RegisterSystem<T>(system);
        }

        template<typename T>
        void SetSystemSignature(Signature signature)
        {
            mSystemManager->SetSignature<T>(signature);
        }

        // Event methods
        void AddEventListener(EventId eventId, std::function<void(Event&)> const& listener)
        {
            mEventManager->AddListener(eventId, listener);
        }

        void SendEvent(Event& event)
        {
            mEventManager->SendEvent(event);
        }

        void SendEvent(EventId eventId)
        {
            mEventManager->SendEvent(eventId);
        }

    private:
        ComponentManager* mComponentManager;
        EntityManager* mEntityManager;
        EventManager* mEventManager;
        SystemManager* mSystemManager;
    };
}
