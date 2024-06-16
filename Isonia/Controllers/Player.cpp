#include "Controllers.h"

namespace Isonia::Controllers
{
    void Player::act(GLFWwindow* window, float frame_time_s)
    {
        m_controller.move(window, frame_time_s, &m_transform);
        m_camera.setView(&m_transform);
    }

    Pipeline::PixelRenderer::EventHandler Player::getOnAspectChangeCallback()
    {
        return [&](Pipeline::PixelRenderer* renderer) { this->onAspectChange(renderer); };
    }

    void Player::onAspectChange(Pipeline::PixelRenderer* renderer)
    {
        m_camera.setProjection(renderer);
    }
}
