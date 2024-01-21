#pragma once

// internal
#include "../ECS/Definitions.h"

// internal
#include "../Renderable/Model.h"

namespace Isonia::Components
{
	struct Mesh : ECS::Archetype<3>
	{
	public:
		Mesh() : model(nullptr)
		{
		}

		Mesh(Renderable::Model* model) : model(model)
		{
		}

		Renderable::Model* model;
	};
}
