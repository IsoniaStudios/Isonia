// internal
#include "Pipeline.h"

namespace Isonia::Pipeline
{
    void Camera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far)
    {
        m_projection_matrix = Math::Matrix4x4{
            2.f / (right - left), 0.f, 0.f, 0.f,
            0.f, 2.f / (bottom - top), 0.f, 0.f,
            0.f, 0.f, 1.f / (far - near), 0.f,
            -(right + left) / (right - left), -(bottom + top) / (bottom - top), -near / (far - near), 1.f
        };
        m_inverse_projection_matrix = Math::mat4Inverse(m_projection_matrix);
        m_aspect = (right - left) / (bottom - top);
    }

    void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far)
    {
        assert(Math::absf(aspect - Math::epsilon) > 0.0f);
        const float tan_half_fov_y = Math::tanf(fovy / 2.f);
        m_projection_matrix = Math::Matrix4x4{
            1.f / (aspect * tan_half_fov_y), 0.f, 0.f, 0.f,
            0.f, 1.f / tan_half_fov_y, 0.f, 0.f,
            0.f, 0.f, far / (far - near), 1.f,
            0.f, 0.f, -(far * near) / (far - near), 0.f
        };
        m_inverse_projection_matrix = Math::mat4Inverse(m_projection_matrix);
        m_aspect = aspect;
    }

    void Camera::setViewDirection(const Math::Vector3* position, const  Math::Vector3* direction, const Math::Vector3* up)
    {
        const Math::Vector3 w = Math::vec3Normalize(direction);
        const Math::Vector3 c = Math::vec3Cross(&w, up);
        const Math::Vector3 u = Math::vec3Normalize(&c);
        const Math::Vector3 v = Math::vec3Cross(&w, &u);

        m_view_matrix = Math::Matrix4x4{
            u.x, v.x, w.x, 0.f,
            u.y, v.y, w.y, 0.f,
            u.z, v.z, w.z, 0.f,
            -Math::vec3Dot(&u, position),
            -Math::vec3Dot(&v, position),
            -Math::vec3Dot(&w, position),
            1.f
        };

        m_inverse_view_matrix = Math::Matrix4x4{
            u.x, v.x, w.x, 0.f,
            u.y, v.y, w.y, 0.f,
            u.z, v.z, w.z, 0.f,
            position->x, position->y, position->z, 1.f
        };
    }

    void Camera::setViewTarget(const Math::Vector3* position, const Math::Vector3* target, const Math::Vector3* up)
    {
        const Math::Vector3 direction = vec3Sub(target, position);
        setViewDirection(position, &direction, up);
    }

    void Camera::setView(Math::Transform* transform)
    {
        setViewYXZ(&transform->position, &transform->rotation);
    }

    void Camera::setProjection(Renderer* renderer)
    {
        const float aspect = renderer->getAspectRatio();
        setPerspectiveProjection(Math::radiansf(50.f), aspect, 0.1f, 1'000.f);
    }

    const Math::Matrix4x4* Camera::getProjection() const
    {
        return &m_projection_matrix;
    }
    const Math::Matrix4x4* Camera::getView() const
    {
        return &m_view_matrix;
    }
    const Math::Matrix4x4* Camera::getInverseView() const
    {
        return &m_inverse_view_matrix;
    }
    const Math::Vector3 Camera::getForwardVector() const
    {
        return Math::Vector3{ m_inverse_view_matrix[2][0], m_inverse_view_matrix[2][1], m_inverse_view_matrix[2][2] };
    }
    const Math::Vector3 Camera::getUpVector() const
    {
        return Math::Vector3{ m_inverse_view_matrix[1][0], m_inverse_view_matrix[1][1], m_inverse_view_matrix[1][2] };
    }
    const Math::Vector3 Camera::getRightVector() const
    {
        return Math::Vector3{ m_inverse_view_matrix[0][0], m_inverse_view_matrix[0][1], m_inverse_view_matrix[0][2] };
    }
    const Math::Vector3 Camera::getPositionVector() const
    {
        return Math::Vector3{ m_inverse_view_matrix[3][0], m_inverse_view_matrix[3][1], m_inverse_view_matrix[3][2] };
    }

    void Camera::setViewYXZ(const Math::Vector3* position, const Math::Vector3* rotation)
    {
        const float c3 = Math::cosf(rotation->z);
        const float s3 = Math::sinf(rotation->z);
        const float c2 = Math::cosf(rotation->x);
        const float s2 = Math::sinf(rotation->x);
        const float c1 = Math::cosf(rotation->y);
        const float s1 = Math::sinf(rotation->y);
        const Math::Vector3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
        const Math::Vector3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
        const Math::Vector3 w{ (c2 * s1), (-s2), (c1 * c2) };
        m_view_matrix = Math::Matrix4x4{
            u.x, v.x, w.x, 0.f,
            u.y, v.y, w.y, 0.f,
            u.z, v.z, w.z, 0.f,
            -Math::vec3Dot(&u, position),
            -Math::vec3Dot(&v, position),
            -Math::vec3Dot(&w, position),
            1.f
        };

        m_inverse_view_matrix = Math::Matrix4x4{
            u.x, u.y, u.z, 0.f,
            v.x, v.y, v.z, 0.f,
            w.x, w.y, w.z, 0.f,
            position->x, position->y, position->z, 1.f
        };
    }

    bool Camera::inFrustum(const Math::BoundingPlane* plane) const
    {
        return false;
    }

    float Camera::getAspect() const
    {
        return m_aspect;
    }

    Math::Ray Camera::ndcToRay(float ndc_x, float ndc_y) const
    {
        // Convert NDC to homogeneous clip space
        Math::Vector4 clip_space_pos(ndc_x, ndc_x, -1.0f, 1.0f);

        // Transform from clip space to view space
        Math::Vector4 view_space_position = Math::mat4Mul(m_inverse_projection_matrix, &clip_space_pos);
        Math::Vector4 view_space_position_normalized = Math::vec4Div(&view_space_position, view_space_position.w);

        // Transform from view space to world space
        Math::Vector4 world_space_position = Math::mat4Mul(m_inverse_view_matrix, &view_space_position_normalized);
        Math::Vector3 world_space_dir = Math::vec3Normalize(world_space_position.x, world_space_position.y, world_space_position.z);

        // Create and return the ray
        return Math::Ray{ world_space_position, world_space_dir };
    }
}
