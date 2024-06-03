#pragma once

// internal
#include "KeyboardController.h"
#include "../Window/Window.h"
#include "../Components/Transform.h"
#include "../Utilities/MathUtility.h"
#include "../Math/Retro.h"

// std
#include <limits>

// external
#include <glm/gtc/matrix_transform.hpp>

namespace Isonia::Controllers
{
	class KeyboardControllerIsometric : KeyboardController
	{
	public:
		void Move(GLFWwindow* window, float dt, Components::Transform* transform) override
		{
			glm::vec3 rotate{ 0 };
			if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y -= 1.f;
			if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y += 1.f;

			if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
			{
				transform->rotation += lookSpeed * dt * glm::normalize(rotate);
			}

			float yaw = transform->rotation.y;
			const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
			const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
			const glm::vec3 upDir{ 0.f, -1.f, 0.f };

			glm::vec3 moveDir{ 0.f };
			if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
			if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
			if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
			if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
			if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
			if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

			if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
			{
				float speedScalar = glfwGetKey(window, keys.walk) == GLFW_PRESS ? walkSpeed : glfwGetKey(window, keys.sprint) == GLFW_PRESS ? sprintSpeed : moveSpeed;
				transform->position += speedScalar * dt * glm::normalize(moveDir);
			}
		}
	};
}
