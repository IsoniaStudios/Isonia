#include "Controllers.h"

namespace Isonia::Controllers
{
    void KeyboardControllerIsometric::move(GLFWwindow* window, float dt, Components::Transform* transform)
    {
        Math::Vector3 rotate{ 0 };
        if (glfwGetKey(window, keys.look_right) == GLFW_PRESS) rotate.y -= 1.f;
        if (glfwGetKey(window, keys.look_left) == GLFW_PRESS) rotate.y += 1.f;

        if (Math::dot(rotate, rotate) > Math::epsilon)
        {
            transform->rotation += m_look_speed * dt * Math::vec3Normalize(rotate);
        }

        float yaw = transform->rotation.y;
        const Math::Vector3 forward_dir{ Math::sinf(yaw), 0.f, Math::cosf(yaw) };
        const Math::Vector3 right_dir{ forward_dir.z, 0.f, -forward_dir.x };
        const Math::Vector3 up_dir{ 0.f, -1.f, 0.f };

        Math::Vector3 moveDir{ 0.f };
        if (glfwGetKey(window, keys.move_forward) == GLFW_PRESS) move_dir += forward_dir;
        if (glfwGetKey(window, keys.move_backward) == GLFW_PRESS) move_dir -= forward_dir;
        if (glfwGetKey(window, keys.move_right) == GLFW_PRESS) move_dir += right_dir;
        if (glfwGetKey(window, keys.move_left) == GLFW_PRESS) move_dir -= right_dir;
        if (glfwGetKey(window, keys.move_up) == GLFW_PRESS) move_dir += up_dir;
        if (glfwGetKey(window, keys.move_down) == GLFW_PRESS) move_dir -= up_dir;

        if (Math::dot(move_dir, move_dir) > Math::epsilon)
        {
            float speed_scalar = glfwGetKey(window, keys.walk) == GLFW_PRESS ? m_walk_speed : glfwGetKey(window, keys.sprint) == GLFW_PRESS ? m_sprint_speed : m_move_speed;
            transform->position += speed_scalar * dt * Math::vec3Normalize(move_dir);
        }
    }
}
