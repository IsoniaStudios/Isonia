#pragma once

// internal
#include "System.h"
#include "Definitions.h"

// std
#include <cassert>
#include <memory>
#include <unordered_map>
#include <typeindex>

namespace Isonia::ECS
{
	class SystemManager
	{
	public:
        template<typename T>
        T* RegisterSystem()
        {
            const char* typeName = typeid(T).name();
            assert(systems.find(typeName) == systems.end() && "Registering system more than once.");
            auto system = new T();
            systems.insert({ typeName, system });
            return system;
        }

        template<typename T>
        T* RegisterSystem(T* system)
        {
            const char* typeName = typeid(T).name();
            assert(systems.find(typeName) == systems.end() && "Registering system more than once.");
            systems.insert({ typeName, system });
            return system;
        }

        template<typename T>
        void SetSignature(Signature signature)
        {
            const char* typeName = typeid(T).name();
            assert(systems.find(typeName) != systems.end() && "System used before registered.");
            signatures.insert({ typeName, signature });
        }

        void EntityDestroyed(Entity entity)
        {
            for (auto const& pair : systems)
            {
                auto const& system = pair.second;
                system->entities.erase(entity);
            }
        }

        void EntitySignatureChanged(Entity entity, Signature entitySignature)
        {
            for (auto const& pair : systems)
            {
                auto const& type = pair.first;
                auto const& system = pair.second;
                auto const& systemSignature = signatures[type];

                if ((entitySignature & systemSignature) == systemSignature)
                {
                    system->entities.insert(entity);
                }
                else
                {
                    system->entities.erase(entity);
                }
            }
        }

	private:
		std::unordered_map<const char*, Signature> signatures{};
        std::unordered_map<const char*, System*> systems{};
    };
}
