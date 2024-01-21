#pragma once

// internal
#include "VertexXZUniform.h"

// std
#include <stdexcept>
#include <bitset>

namespace Isonia::Renderable
{
	const std::size_t STRIP_COUNT = 8;
	const std::size_t VERTICES_PER_STRIP = (STRIP_COUNT + 1) * 2;
	const std::size_t VERTICES = VERTICES_PER_STRIP * STRIP_COUNT;
	const float UNIFORM_DISTANCE = 0.1f;

	struct BuilderXZUniform
	{
		VertexXZUniform vertices[VERTICES];

		BuilderXZUniform()
		{
			for (size_t i = 0; i < VERTICES; i++)
			{
				vertices[i] = VertexXZUniform{};
			}
		}
	};
}
