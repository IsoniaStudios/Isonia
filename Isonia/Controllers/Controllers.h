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
            KeyMappings() = default;
            KeyMappings(const KeyMappings&) = delete;
            KeyMappings& operator=(const KeyMappings&) = delete;

            unsigned int move_left = Pipeline::KeyCodes::a;
            unsigned int move_right = Pipeline::KeyCodes::d;
            unsigned int move_forward = Pipeline::KeyCodes::w;
            unsigned int move_backward = Pipeline::KeyCodes::s;
            unsigned int move_up = Pipeline::KeyCodes::e;
            unsigned int move_down = Pipeline::KeyCodes::q;

            unsigned int sprint = Pipeline::KeyCodes::left_shift;
            unsigned int walk = Pipeline::KeyCodes::left_control;

            unsigned int look_left = Pipeline::KeyCodes::left;
            unsigned int look_right = Pipeline::KeyCodes::right;
            unsigned int look_up = Pipeline::KeyCodes::up;
            unsigned int look_down = Pipeline::KeyCodes::down;

            unsigned int perspective = Pipeline::KeyCodes::tab;
        };

        KeyboardController() = default;
        KeyboardController(const KeyboardController&) = delete;
        KeyboardController& operator=(const KeyboardController&) = delete;

        virtual void move(Pipeline::Window* window, float dt, Math::Transform* transform);

        KeyMappings m_keys{};
        float m_sprint_speed{ 60.f };
        float m_move_speed{ 30.0f };
        float m_walk_speed{ 0.3f };
        float m_look_speed{ 3.0f };
    };

    struct KeyboardControllerIsometric : public KeyboardController
    {
    public:
        KeyboardControllerIsometric() = default;
        KeyboardControllerIsometric(const KeyboardControllerIsometric&) = delete;
        KeyboardControllerIsometric& operator=(const KeyboardControllerIsometric&) = delete;

        void move(Pipeline::Window* window, float dt, Math::Transform* transform) override;
    };

    struct Player
    {
    public:
        Player();
        ~Player();

        Player(const Player&) = delete;
        Player& operator=(const Player&) = delete;

        void act(Pipeline::Window* window, float frame_time_s);

        Pipeline::Renderer::EventHandler getOnAspectChangeCallback();
        static void onAspectChange(Pipeline::Renderer* renderer, void* user_data);

        Math::Transform m_transform{};
        Pipeline::Camera m_camera{};
        KeyboardController m_controller{};
    };

    struct PlayerIsometric
    {
    public:
        PlayerIsometric();
        ~PlayerIsometric();

        PlayerIsometric(const PlayerIsometric&) = delete;
        PlayerIsometric& operator=(const PlayerIsometric&) = delete;

        void act(Pipeline::Window* window, float frame_time_s);

        Pipeline::PixelRenderer::EventHandler getOnAspectChangeCallback();
        static void onAspectChange(Pipeline::PixelRenderer* renderer, void* user_data);

        Math::Transform m_transform{};
        Pipeline::CameraIsometric m_camera{};
        KeyboardControllerIsometric m_controller{};
    };
}
