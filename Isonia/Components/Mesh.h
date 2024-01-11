#pragma once

// internal
#include "../Pipeline/Model.h"

namespace Isonia::Components
{
	struct Mesh
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
