#pragma once

#include "System.h"
#include "Definitions.h"
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
        std::shared_ptr<T> RegisterSystem()
        {
            const char* typeName = typeid(T).name();
            assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");
            auto system = std::make_shared<T>();
            mSystems.insert({ typeName, system });
            return system;
        }

        template<typename T>
        std::shared_ptr<T> RegisterSystem(T* system)
        {
            const char* typeName = typeid(T).name();
            assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");
            std::shared_ptr<T> systemPtr(system);
            mSystems.insert({ typeName, systemPtr });
            return systemPtr;
        }

        template<typename T>
        void SetSignature(Signature signature)
        {
            const char* typeName = typeid(T).name();
            assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");
            mSignatures.insert({ typeName, signature });
        }

		void EntityDestroyed(Entity entity);

		void EntitySignatureChanged(Entity entity, Signature entitySignature);

	private:
		std::unordered_map<const char*, Signature> mSignatures{};
		std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};
	};
}
