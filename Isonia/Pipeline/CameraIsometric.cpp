#include "Pipeline.h"

namespace Isonia::Pipeline
{
    void CameraIsometric::setView(Components::Transform* transform)
    {
        // set final rotation
        transform->rotation.x = Math::radiansf(-30.0f);

        // get matrices for calculations
        Math::Matrix4x4 camera_rotation = transform->normalMatrix();
        Math::Matrix4x4 inverse_camera_rotation = Math::mat4Inverse(camera_rotation);

        // get local position
        Math::Vector3 local_position = transform->position;

        Math::Vector3 camera_position = camera_rotation * Math::Vector4{ 0.0f, 0.0f, -camera_distance, m_pixel_global_top_left.w };

        // rotate offset from global to local
        Math::Vector4 pixelLocalTopLeft = camera_rotation * m_pixel_global_top_left;

        // offset by pixelLocalTopLeft to get world
        Math::Vector4 pixelWorldTopLeft
        {
            local_position.x + pixelLocalTopLeft.x,
            local_position.y + pixelLocalTopLeft.y,
            local_position.z + pixelLocalTopLeft.z,
            pixelLocalTopLeft.w
        };

        // rotate back to grid
        Math::Vector4 m_pixel_global_top_left = inverse_camera_rotation * pixelWorldTopLeft;

        // now we can snap it to the global pixel grid
        Math::Vector4 roundedm_pixel_global_top_left = Math::Retro::RoundToPixel(m_pixel_global_top_left);

        // rotate it back again
        Math::Vector4 roundedPixelWorldTopLeft = camera_rotation * roundedm_pixel_global_top_left;

        // get the rounded and unrounded pixel position difference
        Math::Vector3 difference = roundedPixelWorldTopLeft - pixelWorldTopLeft;

        // final position
        Math::Vector3 position = local_position + difference + camera_position;

        // set view and position
        SetViewYXZ(position, transform->rotation);

        // from difference get render offset by rotating back
        Math::Vector3 unrotatedDifference = inverse_camera_rotation * Math::Vector4{ difference, 1.0 };

        // from now on dismiss z
        subPixelOffset.x = unrotatedDifference.x;
        subPixelOffset.y = unrotatedDifference.y;
    }

    void CameraIsometric::setProjection(PixelRenderer* renderer)
    {
        // get extent
        const auto extent = renderer->getExtent();
        // get ortho size
        const float ortho_size = Math::pixels_per_unit * 2.0;
        // set projection
        SetOrthographicProjection(
            -(extent.width / ortho_size),
            extent.width / ortho_size,
            -(extent.height / ortho_size),
            extent.height / ortho_size,
            0.f,
            1000.f
        );

        // NDC coordinates of the top left upmost center pixel position
        Math::Vector4 ndc_top_left{ -1.0f, -1.0f, 0.0f, 1.0f };
        // apply inverse projection matrix
        Math::Vector4 global_top_left = m_inverse_projection_matrix * ndc_top_left;
        // normalize by dividing by w
        global_top_left /= global_top_left.w;
        // pushbach camera distance
        global_top_left.z -= camera_distance;

        // push to center pixel position
        m_pixel_global_top_left = {
            global_top_left.x + Math::units_per_pixel * 0.5f,
            global_top_left.y + Math::units_per_pixel * 0.5f,
            global_top_left.z + Math::units_per_pixel * 0.5f,
            global_top_left.w
        };
    }
}
