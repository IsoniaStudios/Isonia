#pragma once

// internal
#include "KeyboardController.h"
#include "../Window/Window.h"
#include "../Components/Transform.h"
#include "../Math/Vector.h"

// std
#include <limits>

namespace Isonia::Controllers
{
	class KeyboardControllerIsometric : KeyboardController
	{
	public:
		void Move(GLFWwindow* window, float dt, Components::Transform* transform) override
		{
			Math::Vector3 rotate{ 0 };
			if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y -= 1.f;
			if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y += 1.f;

			if (Math::Dot(rotate, rotate) > Math::Epsilon)
			{
				transform->rotation += lookSpeed * dt * Math::Normalize(rotate);
			}

			float yaw = transform->rotation.y;
			const Math::Vector3 forwardDir{ Math::Sin(yaw), 0.f, Math::Cos(yaw) };
			const Math::Vector3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
			const Math::Vector3 upDir{ 0.f, -1.f, 0.f };

			Math::Vector3 moveDir{ 0.f };
			if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
			if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
			if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
			if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
			if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
			if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

			if (Math::Dot(moveDir, moveDir) > Math::Epsilon)
			{
				float speedScalar = glfwGetKey(window, keys.walk) == GLFW_PRESS ? walkSpeed : glfwGetKey(window, keys.sprint) == GLFW_PRESS ? sprintSpeed : moveSpeed;
				transform->position += speedScalar * dt * Math::Normalize(moveDir);
			}
		}
	};
}
