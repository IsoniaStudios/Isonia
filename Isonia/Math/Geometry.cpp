// internal
#include "Math.h"

namespace Isonia::Math
{
	Ray::Ray()
	{
	}
	Ray::Ray(Vector3 origin, Vector3 direction)
	{
		this->origin = origin;
		this->direction = direction;
	}
	Ray::Ray(Vector4 origin, Vector3 direction)
	{
		this->origin = Vector3{ origin.x, origin.y, origin.z };
		this->direction = direction;
	}

	Plane::Plane()
	{
	}
	Plane::Plane(Vector3 origin, Vector3 normal)
	{
		this->origin = origin;
		this->normal = normal;
	}

	extern inline constexpr bool intersectRayPlane(const Ray* ray, const Plane* plane, float* intersection_distance)
	{
		return intersectRayPlane(&ray->origin, &ray->direction, &plane->origin, &plane->normal, intersection_distance);
	}
	extern inline constexpr bool intersectRayPlane(const Vector3* origin, const Vector3* direction, const Vector3* plane_origin, const Vector3* plane_normal, float* intersection_distance)
	{
		const float dot = vec3Dot(direction, plane_normal);
		if (absf(dot) > epsilon)
		{
			const Vector3 origin_differance = vec3Sub(plane_origin, origin);
			const float tmp_intersection_distance = vec3Dot(&origin_differance, plane_normal) / dot;
			if (tmp_intersection_distance > 0.0f)
			{
				*intersection_distance = tmp_intersection_distance;
				return true;
			}
		}
		return false;
	}
}
