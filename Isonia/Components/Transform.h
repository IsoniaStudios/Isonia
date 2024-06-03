#pragma once

// internal
#include "../ECS/Definitions.h"
#include "../Math/Trigonometry.h"
#include "../Math/Matrix.h"
#include "../Math/Vector.h"

namespace Isonia::Components
{
	struct Transform : ECS::Archetype<0>
	{
	public:
		Math::Vector3 position;
		Math::Vector3 rotation;
		Math::Vector3 scale;

		Transform() : position(0.f, 0.f, 0.f), rotation(0.f, 0.f, 0.f), scale(1.f, 1.f, 1.f)
		{
		}

		Transform(Math::Vector3 initialPosition, Math::Vector3 initialRotation, Math::Vector3 initialScale)
			: position(initialPosition), rotation(initialRotation), scale(initialScale)
		{
		}

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		const Math::Matrix4x4 Mat4() const
		{
			const float c3 = Math::Cos(rotation.z);
			const float s3 = Math::Sin(rotation.z);
			const float c2 = Math::Cos(rotation.x);
			const float s2 = Math::Sin(rotation.x);
			const float c1 = Math::Cos(rotation.y);
			const float s1 = Math::Sin(rotation.y);
			return Math::Matrix4x4{
				{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f,
				},
				{position.x, position.y, position.z, 1.0f}
			};
		}

		const Math::Matrix3x3 NormalMatrix() const
		{
			const float c3 = Math::Cos(rotation.z);
			const float s3 = Math::Sin(rotation.z);
			const float c2 = Math::Cos(rotation.x);
			const float s2 = Math::Sin(rotation.x);
			const float c1 = Math::Cos(rotation.y);
			const float s1 = Math::Sin(rotation.y);
			const Math::Vector3 invScale = 1.0f / scale;

			return Math::Matrix3x3{
				{
					invScale.x * (c1 * c3 + s1 * s2 * s3),
					invScale.x * (c2 * s3),
					invScale.x * (c1 * s2 * s3 - c3 * s1),
				},
				{
					invScale.y * (c3 * s1 * s2 - c1 * s3),
					invScale.y * (c2 * c3),
					invScale.y * (c1 * c3 * s2 + s1 * s3),
				},
				{
					invScale.z * (c2 * s1),
					invScale.z * (-s2),
					invScale.z * (c1 * c2),
				},
			};
		}
	};
}
