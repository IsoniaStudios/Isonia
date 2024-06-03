#pragma once

// external
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/hash.hpp>

namespace Isonia::Math
{
	using Vector1 = glm::vec1;
	using Vector2 = glm::vec2;
	using Vector3 = glm::vec3;
	using Vector4 = glm::vec4;

	Vector1 Normalize(Vector1 v) { return glm::normalize(v); }
	Vector2 Normalize(Vector2 v) { return glm::normalize(v); }
	Vector3 Normalize(Vector3 v) { return glm::normalize(v); }
	Vector4 Normalize(Vector4 v) { return glm::normalize(v); }

	Vector3 Cross(Vector3 v1, Vector3 v2) { return glm::cross(v1, v2); }

	float Dot(Vector1 v1, Vector1 v2) { return glm::dot(v1, v2); }
	float Dot(Vector2 v1, Vector2 v2) { return glm::dot(v1, v2); }
	float Dot(Vector3 v1, Vector3 v2) { return glm::dot(v1, v2); }
	float Dot(Vector4 v1, Vector4 v2) { return glm::dot(v1, v2); }

	bool IntersectRayPlane(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, float& f) { return glm::intersectRayPlane(v1, v2, v3, v4, f); }

	static inline float Atan(const float y, const float x) { return glm::atan(y, x); }
}
