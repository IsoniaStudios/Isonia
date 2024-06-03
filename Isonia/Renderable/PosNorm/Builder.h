#pragma once

// internal
#include "Vertex.h"

#include "../XZUniformN/Builder.h"

#include "../../Noise/Noise.h"

#include "../../Pipeline/Buffer.h"
#include "../../Pipeline/Device.h"

// std
#include <stdexcept>
#include <bitset>

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
		Vertex* vertices;

		Builder(Renderable::XZUniformN::Builder* XZUniformN)
		{
		}
	};
}
