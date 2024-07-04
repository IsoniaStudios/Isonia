// internal
#include "Math.h"

namespace Isonia::Math
{
	Transform::Transform()
		: position(0.f, 0.f, 0.f), rotation(0.f, 0.f, 0.f), scale(1.f, 1.f, 1.f)
	{
	}

	Transform::Transform(Math::Vector3 initial_position, Math::Vector3 initial_rotation, Math::Vector3 initial_scale)
		: position(initial_position), rotation(initial_rotation), scale(initial_scale)
	{
	}

	const Math::Matrix4x4 Transform::matrix4x4() const
	{
		const float c3 = Math::cosf(rotation.z);
		const float s3 = Math::sinf(rotation.z);
		const float c2 = Math::cosf(rotation.x);
		const float s2 = Math::sinf(rotation.x);
		const float c1 = Math::cosf(rotation.y);
		const float s1 = Math::sinf(rotation.y);
		return Math::Matrix4x4{
			scale.x * (c1 * c3 + s1 * s2 * s3),
			scale.x * (c2 * s3),
			scale.x * (c1 * s2 * s3 - c3 * s1),
			0.0f,

			scale.y * (c3 * s1 * s2 - c1 * s3),
			scale.y * (c2 * c3),
			scale.y * (c1 * c3 * s2 + s1 * s3),
			0.0f,

			scale.z * (c2 * s1),
			scale.z * (-s2),
			scale.z * (c1 * c2),
			0.0f,

			position.x,
			position.y,
			position.z,
			1.0f
		};
	}

	const Math::Matrix3x3 Transform::normalMatrix3x3() const
	{
		const float c3 = Math::cosf(rotation.z);
		const float s3 = Math::sinf(rotation.z);
		const float c2 = Math::cosf(rotation.x);
		const float s2 = Math::sinf(rotation.x);
		const float c1 = Math::cosf(rotation.y);
		const float s1 = Math::sinf(rotation.y);
		const Math::Vector3 invScale = Math::vec3Div(1.0f, &scale);

		return Math::Matrix3x3{
			invScale.x * (c1 * c3 + s1 * s2 * s3),
			invScale.x * (c2 * s3),
			invScale.x * (c1 * s2 * s3 - c3 * s1),

			invScale.y * (c3 * s1 * s2 - c1 * s3),
			invScale.y * (c2 * c3),
			invScale.y * (c1 * c3 * s2 + s1 * s3),

			invScale.z * (c2 * s1),
			invScale.z * (-s2),
			invScale.z * (c1 * c2),
		};
	}
}
