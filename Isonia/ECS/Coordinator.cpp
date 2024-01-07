#include "Coordinator.h"

namespace Isonia::ECS
{
    void Coordinator::Init()
    {
        mComponentManager = new ComponentManager();
        mEntityManager = new EntityManager();
        mEventManager = new EventManager();
        mSystemManager = new SystemManager();
    }

    // Entity methods
    Entity Coordinator::CreateEntity()
    {
        return mEntityManager->CreateEntity();
    }

    void Coordinator::DestroyEntity(Entity entity)
    {
        mEntityManager->DestroyEntity(entity);
        mComponentManager->EntityDestroyed(entity);
        mSystemManager->EntityDestroyed(entity);
    }

    // Event methods
    void Coordinator::AddEventListener(EventId eventId, std::function<void(Event&)> const& listener)
    {
        mEventManager->AddListener(eventId, listener);
    }

    void Coordinator::SendEvent(Event& event)
    {
        mEventManager->SendEvent(event);
    }

    void Coordinator::SendEvent(EventId eventId)
    {
        mEventManager->SendEvent(eventId);
    }
}
