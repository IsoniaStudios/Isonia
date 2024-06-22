// internal
#include "Pipeline.h"

namespace Isonia::Pipeline
{
    void Camera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far)
    {
        m_projection_matrix = Math::Matrix4x4{ 1.0f };
        m_projection_matrix[0][0] = 2.f / (right - left);
        m_projection_matrix[1][1] = 2.f / (bottom - top);
        m_projection_matrix[2][2] = 1.f / (far - near);
        m_projection_matrix[3][0] = -(right + left) / (right - left);
        m_projection_matrix[3][1] = -(bottom + top) / (bottom - top);
        m_projection_matrix[3][2] = -near / (far - near);
        m_inverse_projection_matrix = Math::mat4Inverse(m_projection_matrix);
    }

    void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far)
    {
        assert(Math::absf(aspect - Math::epsilon) > 0.0f);
        const float tan_half_fov_y = Math::tanf(fovy / 2.f);
        m_projection_matrix = Math::Matrix4x4{ 0.0f };
        m_projection_matrix[0][0] = 1.f / (aspect * tan_half_fov_y);
        m_projection_matrix[1][1] = 1.f / (tan_half_fov_y);
        m_projection_matrix[2][2] = far / (far - near);
        m_projection_matrix[2][3] = 1.f;
        m_projection_matrix[3][2] = -(far * near) / (far - near);
        m_inverse_projection_matrix = Math::mat4Inverse(m_projection_matrix);
    }

    void Camera::setViewDirection(const Math::Vector3* position, const  Math::Vector3* direction, const Math::Vector3* up)
    {
        const Math::Vector3 w = Math::vec3Normalize(direction);
        const Math::Vector3 c = Math::vec3Cross(&w, up);
        const Math::Vector3 u = Math::vec3Normalize(&c);
        const Math::Vector3 v = Math::vec3Cross(&w, &u);

        m_view_matrix = Math::Matrix4x4{ 1.f };
        m_view_matrix[0][0] = u.x;
        m_view_matrix[1][0] = u.y;
        m_view_matrix[2][0] = u.z;
        m_view_matrix[0][1] = v.x;
        m_view_matrix[1][1] = v.y;
        m_view_matrix[2][1] = v.z;
        m_view_matrix[0][2] = w.x;
        m_view_matrix[1][2] = w.y;
        m_view_matrix[2][2] = w.z;
        m_view_matrix[3][0] = -Math::vec3Dot(&u, position);
        m_view_matrix[3][1] = -Math::vec3Dot(&v, position);
        m_view_matrix[3][2] = -Math::vec3Dot(&w, position);

        m_inverse_view_matrix = Math::Matrix4x4{ 1.f };
        m_inverse_view_matrix[0][0] = u.x;
        m_inverse_view_matrix[0][1] = u.y;
        m_inverse_view_matrix[0][2] = u.z;
        m_inverse_view_matrix[1][0] = v.x;
        m_inverse_view_matrix[1][1] = v.y;
        m_inverse_view_matrix[1][2] = v.z;
        m_inverse_view_matrix[2][0] = w.x;
        m_inverse_view_matrix[2][1] = w.y;
        m_inverse_view_matrix[2][2] = w.z;
        m_inverse_view_matrix[3][0] = position->x;
        m_inverse_view_matrix[3][1] = position->y;
        m_inverse_view_matrix[3][2] = position->z;
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

    void Camera::setProjection(PixelRenderer* renderer)
    {
        const float aspect = renderer->getAspectRatio();
        setPerspectiveProjection(Math::radiansf(50.f), aspect, 0.1f, 1000.f);
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
        m_view_matrix = Math::Matrix4x4{ 1.f };
        m_view_matrix[0][0] = u.x;
        m_view_matrix[1][0] = u.y;
        m_view_matrix[2][0] = u.z;
        m_view_matrix[0][1] = v.x;
        m_view_matrix[1][1] = v.y;
        m_view_matrix[2][1] = v.z;
        m_view_matrix[0][2] = w.x;
        m_view_matrix[1][2] = w.y;
        m_view_matrix[2][2] = w.z;
        m_view_matrix[3][0] = -Math::vec3Dot(&u, position);
        m_view_matrix[3][1] = -Math::vec3Dot(&v, position);
        m_view_matrix[3][2] = -Math::vec3Dot(&w, position);

        m_view_matrix = Math::Matrix4x4{ 1.f };
        m_view_matrix[0][0] = u.x;
        m_view_matrix[0][1] = u.y;
        m_view_matrix[0][2] = u.z;
        m_view_matrix[1][0] = v.x;
        m_view_matrix[1][1] = v.y;
        m_view_matrix[1][2] = v.z;
        m_view_matrix[2][0] = w.x;
        m_view_matrix[2][1] = w.y;
        m_view_matrix[2][2] = w.z;
        m_view_matrix[3][0] = position->x;
        m_view_matrix[3][1] = position->y;
        m_view_matrix[3][2] = position->z;
    }
}
