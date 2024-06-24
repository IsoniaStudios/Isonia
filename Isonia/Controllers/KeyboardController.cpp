// internal
#include "Controllers.h"

namespace Isonia::Controllers
{
    void KeyboardController::move(Pipeline::Window* window, float dt, Math::Transform* transform)
    {
        Math::Vector3 rotate{ 0.0f };
        if (window->getKey(m_keys.look_right) == Pipeline::KeyActions::press) rotate.y += 1.f;
        if (window->getKey(m_keys.look_left) == Pipeline::KeyActions::press) rotate.y -= 1.f;
        if (window->getKey(m_keys.look_up) == Pipeline::KeyActions::press) rotate.x += 1.f;
        if (window->getKey(m_keys.look_down) == Pipeline::KeyActions::press) rotate.x -= 1.f;

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
        if (window->getKey(m_keys.move_forward) == Pipeline::KeyActions::press) move_dir = Math::vec3Add(&move_dir, &forward_dir);
        if (window->getKey(m_keys.move_backward) == Pipeline::KeyActions::press) move_dir = Math::vec3Sub(&move_dir, &forward_dir);
        if (window->getKey(m_keys.move_right) == Pipeline::KeyActions::press) move_dir = Math::vec3Add(&move_dir, &right_dir);
        if (window->getKey(m_keys.move_left) == Pipeline::KeyActions::press) move_dir = Math::vec3Sub(&move_dir, &right_dir);
        if (window->getKey(m_keys.move_up) == Pipeline::KeyActions::press) move_dir = Math::vec3Add(&move_dir, &up_dir);
        if (window->getKey(m_keys.move_down) == Pipeline::KeyActions::press) move_dir = Math::vec3Sub(&move_dir, &up_dir);

        if (Math::vec3Dot(&move_dir, &move_dir) > Math::epsilon)
        {
            float speed_scalar = window->getKey(m_keys.walk) == Pipeline::KeyActions::press ? m_walk_speed : window->getKey(m_keys.sprint) == Pipeline::KeyActions::press ? m_sprint_speed : m_move_speed;
            Math::Vector3 normalized_move_dir = Math::vec3Normalize(&move_dir);
            Math::Vector3 scaled_move_dir = Math::vec3Mul(&normalized_move_dir, speed_scalar);
            Math::Vector3 derivative_move_dir = Math::vec3Mul(&scaled_move_dir, dt);
            transform->position = Math::vec3Add(&transform->position, &derivative_move_dir);
        }
    }

    void KeyboardControllerIsometric::move(Pipeline::Window* window, float dt, Math::Transform* transform)
    {
        Math::Vector3 rotate{ 0.0f };
        if (window->getKey(m_keys.look_right) == Pipeline::KeyActions::press) rotate.y -= 1.f;
        if (window->getKey(m_keys.look_left) == Pipeline::KeyActions::press) rotate.y += 1.f;

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
        if (window->getKey(m_keys.move_forward) == Pipeline::KeyActions::press) move_dir = Math::vec3Add(&move_dir, &forward_dir);
        if (window->getKey(m_keys.move_backward) == Pipeline::KeyActions::press) move_dir = Math::vec3Sub(&move_dir, &forward_dir);
        if (window->getKey(m_keys.move_right) == Pipeline::KeyActions::press) move_dir = Math::vec3Add(&move_dir, &right_dir);
        if (window->getKey(m_keys.move_left) == Pipeline::KeyActions::press) move_dir = Math::vec3Sub(&move_dir, &right_dir);
        if (window->getKey(m_keys.move_up) == Pipeline::KeyActions::press) move_dir = Math::vec3Add(&move_dir, &up_dir);
        if (window->getKey(m_keys.move_down) == Pipeline::KeyActions::press) move_dir = Math::vec3Sub(&move_dir, &up_dir);

        if (Math::vec3Dot(&move_dir, &move_dir) > Math::epsilon)
        {
            float speed_scalar = window->getKey(m_keys.walk) == Pipeline::KeyActions::press ? m_walk_speed : window->getKey(m_keys.sprint) == Pipeline::KeyActions::press ? m_sprint_speed : m_move_speed;
            Math::Vector3 normalized_move_dir = Math::vec3Normalize(&move_dir);
            Math::Vector3 scaled_move_dir = Math::vec3Mul(&normalized_move_dir, speed_scalar);
            Math::Vector3 derivative_move_dir = Math::vec3Mul(&scaled_move_dir, dt);
            transform->position = Math::vec3Add(&transform->position, &derivative_move_dir);
        }
    }
}
