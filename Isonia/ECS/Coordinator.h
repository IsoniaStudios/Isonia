#pragma once

#include "ComponentManager.h"
#include "EntityManager.h"
#include "EventManager.h"
#include "SystemManager.h"
#include "Definitions.h"
#include <memory>
#include <functional>

namespace Isonia::ECS
{
    class Coordinator
    {
    public:
        void Init();

        // Entity methods
        Entity CreateEntity();
        void DestroyEntity(Entity entity);

        // Component methods
        template<typename T>
        void RegisterComponent();

        template<typename T>
        void AddComponent(Entity entity, T component);

        template<typename T>
        void RemoveComponent(Entity entity);

        template<typename T>
        T& GetComponent(Entity entity);

        template<typename T>
        ComponentType GetComponentType();

        // System methods
        template<typename T>
        std::shared_ptr<T> RegisterSystem();

        template<typename T>
        void SetSystemSignature(Signature signature);

        // Event methods
        void AddEventListener(EventId eventId, std::function<void(Event&)> const& listener);

        void SendEvent(Event& event);
        void SendEvent(EventId eventId);

    private:
        std::unique_ptr<ComponentManager> mComponentManager;
        std::unique_ptr<EntityManager> mEntityManager;
        std::unique_ptr<EventManager> mEventManager;
        std::unique_ptr<SystemManager> mSystemManager;
    };
}
