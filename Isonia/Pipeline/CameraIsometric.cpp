// internal
#include "Pipeline.h"

namespace Isonia::Pipeline
{
    void CameraIsometric::setView(Math::Transform* transform)
    {
        // set final rotation
        transform->rotation.x = Math::radiansf(-30.0f);

        // get matrices for calculations
        Math::Matrix4x4 camera_rotation = transform->normalMatrix3x3();
        Math::Matrix4x4 inverse_camera_rotation = Math::mat4Inverse(camera_rotation);

        // get local position
        Math::Vector3 local_position = transform->position;

        Math::Vector4 camera_offset_position{ 0.0f, 0.0f, -camera_distance, m_pixel_global_top_left.w };
        Math::Vector4 camera_position_vector4 = Math::mat4Mul(camera_rotation, &camera_offset_position);
        Math::Vector3* camera_position = reinterpret_cast<Math::Vector3*>(&camera_position_vector4);

        // rotate offset from global to local
        Math::Vector4 pixel_local_top_left = Math::mat4Mul(camera_rotation, &m_pixel_global_top_left);

        // offset by pixelLocalTopLeft to get world
        Math::Vector4 pixel_world_top_left
        {
            local_position.x + pixel_local_top_left.x,
            local_position.y + pixel_local_top_left.y,
            local_position.z + pixel_local_top_left.z,
            pixel_local_top_left.w
        };

        // rotate back to grid
        Math::Vector4 m_pixel_global_top_left = Math::mat4Mul(inverse_camera_rotation, &pixel_world_top_left);

        // now we can snap it to the global pixel grid
        Math::Vector4 roundedm_pixel_global_top_left = Math::roundVec4ToPixel(m_pixel_global_top_left);

        // rotate it back again
        Math::Vector4 rounded_pixel_world_top_left = Math::mat4Mul(camera_rotation, &roundedm_pixel_global_top_left);

        // get the rounded and unrounded pixel position difference
        Math::Vector4 difference_vector4 = Math::vec4Sub(&rounded_pixel_world_top_left, &pixel_world_top_left);
        Math::Vector3* difference = reinterpret_cast<Math::Vector3*>(&difference_vector4);

        // final position
        Math::Vector3 position_difference = Math::vec3Add(difference, camera_position);
        Math::Vector3 position = Math::vec3Add(&local_position, &position_difference);

        // set view and position
        setViewYXZ(&position, &transform->rotation);

        // from difference get render offset by rotating back
        Math::Vector4 unrotated_difference = Math::mat4Mul(inverse_camera_rotation, &difference_vector4);

        // from now on dismiss z
        m_sub_pixel_offset.x = unrotated_difference.x;
        m_sub_pixel_offset.y = unrotated_difference.y;
    }

    void CameraIsometric::setProjection(PixelRenderer* renderer)
    {
        // get extent
        const VkExtent2D extent = renderer->getExtent();
        // get ortho size
        const float ortho_size = Math::pixels_per_unit * 2.0;
        // set projection
        setOrthographicProjection(
            m_left = -(extent.width / ortho_size),
            m_right = extent.width / ortho_size,
            m_top = -(extent.height / ortho_size),
            m_bottom = extent.height / ortho_size,
            m_near = 0.0f,
            m_far = 1000.0f
        );

        // NDC coordinates of the top left upmost center pixel position
        Math::Vector4 ndc_top_left{ -1.0f, -1.0f, 0.0f, 1.0f };
        // apply inverse projection matrix
        Math::Vector4 global_top_left = Math::mat4Mul(m_inverse_projection_matrix, &ndc_top_left);
        // normalize by dividing by w
        global_top_left = Math::vec4Div(&global_top_left, global_top_left.w);
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

    bool CameraIsometric::inFustrum(Math::BoundingPlane plane) const
    {
        Math::Vector3 half_size(plane.size.x * 0.5f, 0.0f, plane.size.y * 0.5f);
        Math::Vector4 corners[4] = {
            { plane.position.x - half_size.x, plane.position.y, plane.position.z - half_size.z, 1.0f },
            { plane.position.x + half_size.x, plane.position.y, plane.position.z - half_size.z, 1.0f },
            { plane.position.x + half_size.x, plane.position.y, plane.position.z + half_size.z, 1.0f },
            { plane.position.x - half_size.x, plane.position.y, plane.position.z + half_size.z, 1.0f },
        };
        for (int i = 0; i < 4; i++)
        {
            corners[i] = Math::mat4Mul(m_view_matrix, &corners[i]);
        }

        // Check if any corner is inside the frustum
        bool anyCornerInside = false;
        bool allCornersOutsideSameSide = true;
        int outsideTop = 0, outsideBottom = 0, outsideLeft = 0, outsideRight = 0;

        for (int i = 0; i < 4; i++)
        {
            if ((corners[i].x >= m_left && corners[i].x <= m_right) &&
                (corners[i].y >= m_top && corners[i].y <= m_bottom))
            {
                anyCornerInside = true;
                break;
            }

            if (corners[i].x < m_left) outsideLeft++;
            if (corners[i].x > m_right) outsideRight++;
            if (corners[i].y < m_top) outsideTop++;
            if (corners[i].y > m_bottom) outsideBottom++;
        }

        if (anyCornerInside)
            return true;

        // Check if all corners are outside on the same side
        if (outsideLeft == 4 || outsideRight == 4 || outsideTop == 4 || outsideBottom == 4)
            return false;

        // Check if the frustum is entirely inside the plane
        if (outsideLeft == 0 && outsideRight == 0 && outsideTop == 0 && outsideBottom == 0)
            return true;

        // At this point, the plane intersects the frustum
        return true;
    }
}
