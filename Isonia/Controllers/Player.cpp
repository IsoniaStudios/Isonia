// internal
#include "Controllers.h"

namespace Isonia::Controllers
{
    Player::Player()
    {
    }

    Player::~Player()
    {
    }

    void Player::act(Pipeline::Window* window, float frame_time_s)
    {
        m_controller.move(window, frame_time_s, &m_transform);
        m_camera.setView(&m_transform);
    }

    Pipeline::PixelRenderer::EventHandler Player::getOnAspectChangeCallback()
    {
        return &Player::onAspectChange;
    }

    void Player::onAspectChange(Pipeline::PixelRenderer* renderer, void* player)
    {
        static_cast<Player*>(player)->m_camera.setProjection(renderer);
    }
}
