// internal
#include "Math.h"

namespace Isonia::Math
{
	BoundingPlane::BoundingPlane()
		: position(0.0f, 0.0f, 0.0f), size(0.0f, 0.0f)
	{
	}

	BoundingPlane::BoundingPlane(Vector3 position, Vector2 size)
		: position(position), size(size)
	{
	}
}
