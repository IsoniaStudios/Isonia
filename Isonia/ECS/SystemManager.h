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
            assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");
            auto system = new T();
            mSystems.insert({ typeName, system });
            return system;
        }

        template<typename T>
        T* RegisterSystem(T* system)
        {
            const char* typeName = typeid(T).name();
            assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");
            mSystems.insert({ typeName, system });
            return system;
        }

        template<typename T>
        void SetSignature(Signature signature)
        {
            const char* typeName = typeid(T).name();
            assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");
            mSignatures.insert({ typeName, signature });
        }

        void EntityDestroyed(Entity entity)
        {
            for (auto const& pair : mSystems)
            {
                auto const& system = pair.second;
                system->mEntities.erase(entity);
            }
        }

        void EntitySignatureChanged(Entity entity, Signature entitySignature)
        {
            for (auto const& pair : mSystems)
            {
                auto const& type = pair.first;
                auto const& system = pair.second;
                auto const& systemSignature = mSignatures[type];

                if ((entitySignature & systemSignature) == systemSignature)
                {
                    system->mEntities.insert(entity);
                }
                else
                {
                    system->mEntities.erase(entity);
                }
            }
        }

	private:
		std::unordered_map<const char*, Signature> mSignatures{};
        std::unordered_map<const char*, System*> mSystems{};
    };
}
