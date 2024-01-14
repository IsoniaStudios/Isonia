#pragma once

// internal
#include "../ECS/Definitions.h"

// internal
#include "../Pipeline/Model.h"

namespace Isonia::Components
{
	struct Mesh : ECS::Archetype<3>
	{
	public:
		Mesh() : model(nullptr)
		{
		}

		Mesh(Pipeline::Model* model) : model(model)
		{
		}

		Pipeline::Model* model;
	};
}
