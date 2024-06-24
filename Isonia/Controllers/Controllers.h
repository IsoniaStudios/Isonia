#pragma once

// internal
#include "../Math/Math.h"
#include "../Pipeline/Pipeline.h"

namespace Isonia::Controllers
{
    struct KeyboardController
    {
    public:
        struct KeyMappings
        {
            int move_left = Pipeline::KeyCodes::a;
            int move_right = Pipeline::KeyCodes::d;
            int move_forward = Pipeline::KeyCodes::w;
            int move_backward = Pipeline::KeyCodes::s;
            int move_up = Pipeline::KeyCodes::e;
            int move_down = Pipeline::KeyCodes::q;

            int sprint = Pipeline::KeyCodes::left_shift;
            int walk = Pipeline::KeyCodes::left_control;

            int look_left = Pipeline::KeyCodes::left;
            int look_right = Pipeline::KeyCodes::right;
            int look_up = Pipeline::KeyCodes::up;
            int look_down = Pipeline::KeyCodes::down;

            int perspective = Pipeline::KeyCodes::tab;
        };

        virtual void move(Pipeline::Window* window, float dt, Math::Transform* transform);

        KeyMappings m_keys{};
        float m_sprint_speed{ 60.f };
        float m_move_speed{ 30.0f };
        float m_walk_speed{ 0.3f };
        float m_look_speed{ 1.5f };
    };

    struct KeyboardControllerIsometric : public KeyboardController
    {
    public:
        void move(Pipeline::Window* window, float dt, Math::Transform* transform) override;
    };

    struct Player
    {
    public:
        Player();
        ~Player();

        Player(const Player&) = delete;
        Player& operator=(const Player&) = delete;

        void act(Pipeline::Window* window, float frameTime);

        Pipeline::PixelRenderer::EventHandler getOnAspectChangeCallback();
        static void onAspectChange(Pipeline::PixelRenderer* renderer, void* user_data);

        Math::Transform m_transform{};
        Pipeline::CameraIsometric m_camera{};
        KeyboardControllerIsometric m_controller{};
    };
}
