#pragma once

// internal
#include "../Window/Window.h"
#include "../Components/Transform.h"
#include "../Math/Vector.h"

namespace Isonia::Controllers
{
	class KeyboardController
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
			int walk = GLFW_KEY_LEFT_CONTROL;

			int lookLeft = GLFW_KEY_LEFT;
			int lookRight = GLFW_KEY_RIGHT;
			int lookUp = GLFW_KEY_UP;
			int lookDown = GLFW_KEY_DOWN;

			int perspective = GLFW_KEY_TAB;
		};

		virtual void Move(GLFWwindow* window, float dt, Components::Transform* transform)
		{
			Math::Vector3 rotate{ 0 };
			if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
			if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
			if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
			if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

			if (Math::Dot(rotate, rotate) > Math::Epsilon)
			{
				transform->rotation += lookSpeed * dt * Math::Normalize(rotate);
			}

			// limit pitch values between about +/- 85ish degrees
			transform->rotation.x = Math::Clamp(transform->rotation.x, -1.5f, 1.5f);
			transform->rotation.y = Math::ClampRadians(transform->rotation.y);

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

		KeyMappings keys{};
		float sprintSpeed{ 60.f };
		float moveSpeed{ 30.0f };
		float walkSpeed{ 0.3f };
		float lookSpeed{ 1.5f };
	};
}
