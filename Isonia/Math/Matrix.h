#pragma once

// external
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Isonia::Math
{
	using Matrix3x3 = glm::mat3;
	using Matrix4x4 = glm::mat4;

	Matrix3x3 Inverse(Matrix3x3 m) { return glm::inverse(m); }
	Matrix4x4 Inverse(Matrix4x4 m) { return glm::inverse(m); }
}
