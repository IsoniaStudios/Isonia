#pragma once

#include "../Pipeline/Pipeline.h"

namespace Isonia::Controllers
{
    class KeyboardController
    {
    public:
        struct KeyMappings
        {
            int move_left = GLFW_KEY_A;
            int move_right = GLFW_KEY_D;
            int move_forward = GLFW_KEY_W;
            int move_backward = GLFW_KEY_S;
            int move_up = GLFW_KEY_E;
            int move_down = GLFW_KEY_Q;

            int sprint = GLFW_KEY_LEFT_SHIFT;
            int walk = GLFW_KEY_LEFT_CONTROL;

            int look_left = GLFW_KEY_LEFT;
            int look_right = GLFW_KEY_RIGHT;
            int look_up = GLFW_KEY_UP;
            int look_down = GLFW_KEY_DOWN;

            int perspective = GLFW_KEY_TAB;
        };

        virtual void move(GLFWwindow* window, float dt, Components::Transform* transform);

        KeyMappings m_keys{};
        float m_sprint_speed{ 60.f };
        float m_move_speed{ 30.0f };
        float m_walk_speed{ 0.3f };
        float m_look_speed{ 1.5f };
    };

    class KeyboardControllerIsometric : public KeyboardController
    {
    public:
        void move(GLFWwindow* window, float dt, Components::Transform* transform) override;
    };

    class Player
    {
    public:
        Player(const Player&) = delete;
        Player& operator=(const Player&) = delete;

        void act(GLFWwindow* window, float frameTime);

        Pipeline::PixelRenderer::EventHandler getOnAspectChangeCallback();
        void onAspectChange(Pipeline::PixelRenderer* renderer);

        Components::Transform m_transform{};
        Components::CameraIsometric m_camera{};
        KeyboardControllerIsometric m_controller{};
    };
}