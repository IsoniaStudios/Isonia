#include "Controllers.h"

namespace Isonia::Controllers
{
    Player::Player()
    {
    }

    Player::~Player()
    {
    }

    void Player::act(GLFWwindow* window, float frame_time_s)
    {
        m_controller.move(window, frame_time_s, &m_transform);
        m_camera.setView(&m_transform);
    }

    Pipeline::PixelRenderer::EventHandler Player::getOnAspectChangeCallback()
    {
        return &Player::onAspectChange;
    }

    void Player::onAspectChange(Pipeline::PixelRenderer* renderer)
    {
        //m_camera.setProjection(renderer);
    }
}
