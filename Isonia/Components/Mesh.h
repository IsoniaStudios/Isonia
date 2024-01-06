#pragma once

#include "../Pipeline/Model.h"
#include "Transform.h"

namespace Isonia::Components
{
	struct Mesh
	{
	public:
		std::shared_ptr<Pipeline::Model> model{};
	};
}
