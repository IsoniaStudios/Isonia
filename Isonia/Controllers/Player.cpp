// internal
#include "Controllers.h"

namespace Isonia::Controllers
{
    Player::Player()
    {
        m_transform.position.y = -11.5;
        m_transform.position.z = -3;
    }

    Player::~Player()
    {
    }

    void Player::act(Pipeline::Window* window, float frame_time_s)
    {
        m_controller.move(window, frame_time_s, &m_transform);
        m_camera.setView(&m_transform);
    }

    Pipeline::Renderer::EventHandler Player::getOnAspectChangeCallback()
    {
        return &Player::onAspectChange;
    }

    void Player::onAspectChange(Pipeline::Renderer* renderer, void* player)
    {
        static_cast<Player*>(player)->m_camera.setProjection(renderer);
    }

    PlayerIsometric::PlayerIsometric()
    {
    }

    PlayerIsometric::~PlayerIsometric()
    {
    }

    void PlayerIsometric::act(Pipeline::Window* window, float frame_time_s)
    {
        m_controller.move(window, frame_time_s, &m_transform);
        m_camera.setView(&m_transform);
    }

    Pipeline::PixelRenderer::EventHandler PlayerIsometric::getOnAspectChangeCallback()
    {
        return &PlayerIsometric::onAspectChange;
    }

    void PlayerIsometric::onAspectChange(Pipeline::PixelRenderer* renderer, void* player)
    {
        static_cast<PlayerIsometric*>(player)->m_camera.setProjection(renderer);
    }
}
