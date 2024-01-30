#pragma once

// internal
#include "Vertex.h"
#include "../../Noise/Noise.h"
#include "../../Pipeline/Buffer.h"
#include "../../Pipeline/Device.h"

// external
#include <glm/glm.hpp>

// std
#include <stdexcept>
#include <bitset>
#include <cmath>

namespace Isonia::Renderable::PosNorm
{
	struct XZPositionalData
	{
		XZPositionalData(const float x, const float z) : x(x), z(z) { }
		const float x;
		const float z;
	};

	struct Builder
	{
		
	};
}
