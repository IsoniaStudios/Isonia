#pragma once

// internal
#include "../Components/Transform.h"
#include "../Window/Window.h"

// std
#include <limits>

// external
#include <glm/gtc/matrix_transform.hpp>

namespace Isonia::Controllers
{
	class KeyboardMovementController
	{
	public:
		struct KeyMappings
		{
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;
			int moveUp = GLFW_KEY_E;
			int moveDown = GLFW_KEY_Q;

			int sprint = GLFW_KEY_LEFT_SHIFT;

			int lookLeft = GLFW_KEY_LEFT;
			int lookRight = GLFW_KEY_RIGHT;
			int lookUp = GLFW_KEY_UP;
			int lookDown = GLFW_KEY_DOWN;

			int perspective = GLFW_KEY_TAB;
		};

		void Move(GLFWwindow* window, float dt, Components::Transform* transform)
		{
			glm::vec3 rotate{ 0 };
			if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
			if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
			if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
			if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

			if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
			{
				transform->rotation += lookSpeed * dt * glm::normalize(rotate);
			}

			// limit pitch values between about +/- 85ish degrees
			transform->rotation.x = glm::clamp(transform->rotation.x, -1.5f, 1.5f);
			transform->rotation.y = glm::mod(transform->rotation.y, glm::two_pi<float>());

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
				float speedScalar = glfwGetKey(window, keys.sprint) == GLFW_PRESS ? sprintSpeed : moveSpeed;
				transform->position += speedScalar * dt * glm::normalize(moveDir);
			}
		}

		KeyMappings keys{};
		float sprintSpeed{ 60.f };
		float moveSpeed{ 30.f };
		float lookSpeed{ 1.5f };
	};
}
