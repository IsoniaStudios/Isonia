#pragma once

// internal
#include "../ECS/Definitions.h"

// internal
#include "../Renderable/Complete/Model.h"

namespace Isonia::Components
{
	struct Mesh : ECS::Archetype<3>
	{
	public:
		Mesh() : model(nullptr)
		{
		}

		Mesh(Renderable::Complete::Model* model) : model(model)
		{
		}

		Renderable::Complete::Model* model;
	};
}
