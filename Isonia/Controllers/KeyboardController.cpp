// internal
#include "Controllers.h"

namespace Isonia::Controllers
{
    void KeyboardController::move(GLFWwindow* window, float dt, Math::Transform* transform)
    {
        Math::Vector3 rotate{ 0.0f };
        if (glfwGetKey(window, m_keys.look_right) == GLFW_PRESS) rotate.y += 1.f;
        if (glfwGetKey(window, m_keys.look_left) == GLFW_PRESS) rotate.y -= 1.f;
        if (glfwGetKey(window, m_keys.look_up) == GLFW_PRESS) rotate.x += 1.f;
        if (glfwGetKey(window, m_keys.look_down) == GLFW_PRESS) rotate.x -= 1.f;

        if (Math::vec3Dot(&rotate, &rotate) > Math::epsilon)
        {
            Math::Vector3 normalized_rotate = Math::vec3Normalize(&rotate);
            Math::Vector3 scaled_rotate = Math::vec3Mul(&normalized_rotate, m_look_speed);
            Math::Vector3 derivative_rotate = Math::vec3Mul(&scaled_rotate, dt);
            transform->rotation = Math::vec3Add(&transform->rotation, &derivative_rotate);
        }

        // limit pitch values between about +/- 85ish degrees
        transform->rotation.x = Math::clampf(transform->rotation.x, -1.5f, 1.5f);
        transform->rotation.y = Math::clampRadiansf(transform->rotation.y);

        float yaw = transform->rotation.y;
        const Math::Vector3 forward_dir{ Math::sinf(yaw), 0.f, Math::cosf(yaw) };
        const Math::Vector3 right_dir{ forward_dir.z, 0.f, -forward_dir.x };
        const Math::Vector3 up_dir{ 0.f, -1.f, 0.f };

        Math::Vector3 move_dir{ 0.f };
        if (glfwGetKey(window, m_keys.move_forward) == GLFW_PRESS) move_dir = Math::vec3Add(&move_dir, &forward_dir);
        if (glfwGetKey(window, m_keys.move_backward) == GLFW_PRESS) move_dir = Math::vec3Sub(&move_dir, &forward_dir);
        if (glfwGetKey(window, m_keys.move_right) == GLFW_PRESS) move_dir = Math::vec3Add(&move_dir, &right_dir);
        if (glfwGetKey(window, m_keys.move_left) == GLFW_PRESS) move_dir = Math::vec3Sub(&move_dir, &right_dir);
        if (glfwGetKey(window, m_keys.move_up) == GLFW_PRESS) move_dir = Math::vec3Add(&move_dir, &up_dir);
        if (glfwGetKey(window, m_keys.move_down) == GLFW_PRESS) move_dir = Math::vec3Sub(&move_dir, &up_dir);

        if (Math::vec3Dot(&move_dir, &move_dir) > Math::epsilon)
        {
            float speed_scalar = glfwGetKey(window, m_keys.walk) == GLFW_PRESS ? m_walk_speed : glfwGetKey(window, m_keys.sprint) == GLFW_PRESS ? m_sprint_speed : m_move_speed;
            Math::Vector3 normalized_move_dir = Math::vec3Normalize(&move_dir);
            Math::Vector3 scaled_move_dir = Math::vec3Mul(&normalized_move_dir, speed_scalar);
            Math::Vector3 derivative_move_dir = Math::vec3Mul(&scaled_move_dir, dt);
            transform->position = Math::vec3Add(&transform->position, &derivative_move_dir);
        }
    }

    void KeyboardControllerIsometric::move(GLFWwindow* window, float dt, Math::Transform* transform)
    {
        Math::Vector3 rotate{ 0.0f };
        if (glfwGetKey(window, m_keys.look_right) == GLFW_PRESS) rotate.y -= 1.f;
        if (glfwGetKey(window, m_keys.look_left) == GLFW_PRESS) rotate.y += 1.f;

        if (Math::vec3Dot(&rotate, &rotate) > Math::epsilon)
        {
            Math::Vector3 normalized_rotate = Math::vec3Normalize(&rotate);
            Math::Vector3 scaled_rotate = Math::vec3Mul(&normalized_rotate, m_look_speed);
            Math::Vector3 derivative_rotate = Math::vec3Mul(&scaled_rotate, dt);
            transform->rotation = Math::vec3Add(&transform->rotation, &derivative_rotate);
        }

        float yaw = transform->rotation.y;
        const Math::Vector3 forward_dir{ Math::sinf(yaw), 0.f, Math::cosf(yaw) };
        const Math::Vector3 right_dir{ forward_dir.z, 0.f, -forward_dir.x };
        const Math::Vector3 up_dir{ 0.f, -1.f, 0.f };

        Math::Vector3 move_dir{ 0.f };
        if (glfwGetKey(window, m_keys.move_forward) == GLFW_PRESS) move_dir = Math::vec3Add(&move_dir, &forward_dir);
        if (glfwGetKey(window, m_keys.move_backward) == GLFW_PRESS) move_dir = Math::vec3Sub(&move_dir, &forward_dir);
        if (glfwGetKey(window, m_keys.move_right) == GLFW_PRESS) move_dir = Math::vec3Add(&move_dir, &right_dir);
        if (glfwGetKey(window, m_keys.move_left) == GLFW_PRESS) move_dir = Math::vec3Sub(&move_dir, &right_dir);
        if (glfwGetKey(window, m_keys.move_up) == GLFW_PRESS) move_dir = Math::vec3Add(&move_dir, &up_dir);
        if (glfwGetKey(window, m_keys.move_down) == GLFW_PRESS) move_dir = Math::vec3Sub(&move_dir, &up_dir);

        if (Math::vec3Dot(&move_dir, &move_dir) > Math::epsilon)
        {
            float speed_scalar = glfwGetKey(window, m_keys.walk) == GLFW_PRESS ? m_walk_speed : glfwGetKey(window, m_keys.sprint) == GLFW_PRESS ? m_sprint_speed : m_move_speed;
            Math::Vector3 normalized_move_dir = Math::vec3Normalize(&move_dir);
            Math::Vector3 scaled_move_dir = Math::vec3Mul(&normalized_move_dir, speed_scalar);
            Math::Vector3 derivative_move_dir = Math::vec3Mul(&scaled_move_dir, dt);
            transform->position = Math::vec3Add(&transform->position, &derivative_move_dir);
        }
    }
}
