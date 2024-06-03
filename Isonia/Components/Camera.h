#pragma once

// internal
#include "Transform.h"
#include "../Pipeline/Renderer.h"
#include "../Pipeline/PixelRenderer.h"
#include "../Math/Trigonometry.h"
#include "../Math/Matrix.h"
#include "../Math/Vector.h"

namespace Isonia::Components
{
	struct Camera
	{
	public:
		void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far)
		{
			projectionMatrix = Math::Matrix4x4{ 1.0f };
			projectionMatrix[0][0] = 2.f / (right - left);
			projectionMatrix[1][1] = 2.f / (bottom - top);
			projectionMatrix[2][2] = 1.f / (far - near);
			projectionMatrix[3][0] = -(right + left) / (right - left);
			projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
			projectionMatrix[3][2] = -near / (far - near);
			inverseProjectionMatrix = Math::Inverse(projectionMatrix);
		}

		void SetPerspectiveProjection(float fovy, float aspect, float near, float far)
		{
			assert(Math::Abs(aspect - Math::Epsilon) > 0.0f);
			const float tanHalfFovy = tan(fovy / 2.f);
			projectionMatrix = Math::Matrix4x4{ 0.0f };
			projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
			projectionMatrix[1][1] = 1.f / (tanHalfFovy);
			projectionMatrix[2][2] = far / (far - near);
			projectionMatrix[2][3] = 1.f;
			projectionMatrix[3][2] = -(far * near) / (far - near);
			inverseProjectionMatrix = Math::Inverse(projectionMatrix);
		}

		void SetViewDirection(Math::Vector3 position, Math::Vector3 direction, Math::Vector3 up)
		{
			const Math::Vector3 w{ Math::Normalize(direction) };
			const Math::Vector3 u{ Math::Normalize(Math::Cross(w, up)) };
			const Math::Vector3 v{ Math::Cross(w, u) };

			viewMatrix = Math::Matrix4x4{ 1.f };
			viewMatrix[0][0] = u.x;
			viewMatrix[1][0] = u.y;
			viewMatrix[2][0] = u.z;
			viewMatrix[0][1] = v.x;
			viewMatrix[1][1] = v.y;
			viewMatrix[2][1] = v.z;
			viewMatrix[0][2] = w.x;
			viewMatrix[1][2] = w.y;
			viewMatrix[2][2] = w.z;
			viewMatrix[3][0] = -Math::Dot(u, position);
			viewMatrix[3][1] = -Math::Dot(v, position);
			viewMatrix[3][2] = -Math::Dot(w, position);

			inverseViewMatrix = Math::Matrix4x4{ 1.f };
			inverseViewMatrix[0][0] = u.x;
			inverseViewMatrix[0][1] = u.y;
			inverseViewMatrix[0][2] = u.z;
			inverseViewMatrix[1][0] = v.x;
			inverseViewMatrix[1][1] = v.y;
			inverseViewMatrix[1][2] = v.z;
			inverseViewMatrix[2][0] = w.x;
			inverseViewMatrix[2][1] = w.y;
			inverseViewMatrix[2][2] = w.z;
			inverseViewMatrix[3][0] = position.x;
			inverseViewMatrix[3][1] = position.y;
			inverseViewMatrix[3][2] = position.z;
		}

		void SetViewTarget(Math::Vector3 position, Math::Vector3 target, Math::Vector3 up)
		{
			SetViewDirection(position, target - position, up);
		}

		virtual void SetView(Components::Transform* transform)
		{
			SetViewYXZ(transform->position, transform->rotation);
		}

		virtual void SetProjection(Pipeline::PixelRenderer* renderer)
		{
			const auto aspect = renderer->GetAspectRatio();
			SetPerspectiveProjection(Math::Radians(50.f), aspect, 0.1f, 1000.f);
		}

		const Math::Matrix4x4& GetProjection() const { return projectionMatrix; }
		const Math::Matrix4x4& GetView() const { return viewMatrix; }
		const Math::Matrix4x4& GetInverseView() const { return inverseViewMatrix; }

		const Math::Vector3 GetForwardVector() const { return Math::Vector3{ inverseViewMatrix[2][0], inverseViewMatrix[2][1], inverseViewMatrix[2][2] }; }
		const Math::Vector3 GetUpVector() const { return Math::Vector3{ inverseViewMatrix[1][0], inverseViewMatrix[1][1], inverseViewMatrix[1][2] }; }
		const Math::Vector3 GetRightVector() const { return Math::Vector3{ inverseViewMatrix[0][0], inverseViewMatrix[0][1], inverseViewMatrix[0][2] }; }

		const Math::Vector3 GetPositionVector() const { return Math::Vector3{ inverseViewMatrix[3][0], inverseViewMatrix[3][1], inverseViewMatrix[3][2] }; }

	protected:
		void SetViewYXZ(Math::Vector3 position, Math::Vector3 rotation)
		{
			const float c3 = Math::Cos(rotation.z);
			const float s3 = Math::Sin(rotation.z);
			const float c2 = Math::Cos(rotation.x);
			const float s2 = Math::Sin(rotation.x);
			const float c1 = Math::Cos(rotation.y);
			const float s1 = Math::Sin(rotation.y);
			const Math::Vector3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
			const Math::Vector3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
			const Math::Vector3 w{ (c2 * s1), (-s2), (c1 * c2) };
			viewMatrix = Math::Matrix4x4{ 1.f };
			viewMatrix[0][0] = u.x;
			viewMatrix[1][0] = u.y;
			viewMatrix[2][0] = u.z;
			viewMatrix[0][1] = v.x;
			viewMatrix[1][1] = v.y;
			viewMatrix[2][1] = v.z;
			viewMatrix[0][2] = w.x;
			viewMatrix[1][2] = w.y;
			viewMatrix[2][2] = w.z;
			viewMatrix[3][0] = -Math::Dot(u, position);
			viewMatrix[3][1] = -Math::Dot(v, position);
			viewMatrix[3][2] = -Math::Dot(w, position);

			inverseViewMatrix = Math::Matrix4x4{ 1.f };
			inverseViewMatrix[0][0] = u.x;
			inverseViewMatrix[0][1] = u.y;
			inverseViewMatrix[0][2] = u.z;
			inverseViewMatrix[1][0] = v.x;
			inverseViewMatrix[1][1] = v.y;
			inverseViewMatrix[1][2] = v.z;
			inverseViewMatrix[2][0] = w.x;
			inverseViewMatrix[2][1] = w.y;
			inverseViewMatrix[2][2] = w.z;
			inverseViewMatrix[3][0] = position.x;
			inverseViewMatrix[3][1] = position.y;
			inverseViewMatrix[3][2] = position.z;
		}

		Math::Matrix4x4 projectionMatrix{ 1.f };
		Math::Matrix4x4 inverseProjectionMatrix{ 1.f };
		Math::Matrix4x4 viewMatrix{ 1.f };
		Math::Matrix4x4 inverseViewMatrix{ 1.f };
	};
}
