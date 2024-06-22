// internal
#include "Math.h"

namespace Isonia::Math
{
	// Matrix3x3
	constexpr Vector3& Matrix3x3::operator[](const unsigned int i) noexcept
	{
		assert(i < 3);
		return this->value[i];
	}
    constexpr const Vector3& Matrix3x3::operator[](const unsigned int i) const noexcept
	{
		assert(i < 3);
		return this->value[i];
	}
	constexpr Matrix3x3& Matrix3x3::operator=(const Matrix3x3& m)
	{
		this->value[0] = m[0];
		this->value[1] = m[1];
		this->value[2] = m[2];
		return *this;
	}
	extern inline constexpr bool operator==(const Matrix3x3& m1, const Matrix3x3& m2)
	{
		return (m1[0] == m2[0]) && (m1[1] == m2[1]) && (m1[2] == m2[2]);
	}
	extern inline constexpr bool operator!=(const Matrix3x3& m1, const Matrix3x3& m2)
	{
		return (m1[0] != m2[0]) || (m1[1] != m2[1]) || (m1[2] != m2[2]);
	}

    extern inline constexpr Matrix3x3 mat3Mul(const Matrix3x3& m, const float scalar)
	{
		Vector3 v0 = vec3Mul(&m[0], scalar);
		Vector3 v1 = vec3Mul(&m[1], scalar);
		Vector3 v2 = vec3Mul(&m[2], scalar);

		return Matrix3x3{ &v0, &v1, &v2 };
	}
    extern inline constexpr Vector3 mat3Mul(const Matrix3x3& m, const Vector3* v)
	{
		return Vector3{
			m[0][0] * v->x + m[1][0] * v->y + m[2][0] * v->z,
			m[0][1] * v->x + m[1][1] * v->y + m[2][1] * v->z,
			m[0][2] * v->x + m[1][2] * v->y + m[2][2] * v->z
		};
	}
    extern inline constexpr Vector3 mat3Mul(const Vector3* v, const Matrix3x3& m)
	{
		return Vector3{
			m[0][0] * v->x + m[0][1] * v->y + m[0][2] * v->z,
			m[1][0] * v->x + m[1][1] * v->y + m[1][2] * v->z,
			m[2][0] * v->x + m[2][1] * v->y + m[2][2] * v->z
		};
	}
    extern inline constexpr Matrix3x3 mat3Mul(const Matrix3x3& m1, const Matrix3x3& m2)
	{
		return Matrix3x3{
			m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
			m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
			m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2],
			m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
			m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
			m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2],
			m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2],
			m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2],
			m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2]
		};
	}

	extern inline constexpr Matrix3x3 mat3Inverse(const Matrix3x3& m)
	{
		const float determinant = (
			+ m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
			- m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
			+ m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2])
		);
		const float oneOverDeterminant = 1.0f / determinant;

		return Matrix3x3{
			+(m[1][1] * m[2][2] - m[2][1] * m[1][2]) * oneOverDeterminant,
			-(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * oneOverDeterminant,
			+(m[1][0] * m[2][1] - m[2][0] * m[1][1]) * oneOverDeterminant,
			-(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * oneOverDeterminant,
			+(m[0][0] * m[2][2] - m[2][0] * m[0][2]) * oneOverDeterminant,
			-(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * oneOverDeterminant,
			+(m[0][1] * m[1][2] - m[1][1] * m[0][2]) * oneOverDeterminant,
			-(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * oneOverDeterminant,
			+(m[0][0] * m[1][1] - m[1][0] * m[0][1]) * oneOverDeterminant
		};
	}

	// Matrix4x4
    constexpr Vector4& Matrix4x4::operator[](const unsigned int i) noexcept
	{
        assert(i < 4);
		return this->value[i];
	}
    constexpr const Vector4& Matrix4x4::operator[](const unsigned int i) const noexcept
	{
		assert(i < 4);
		return this->value[i];
	}
	constexpr Matrix4x4& Matrix4x4::operator=(const Matrix4x4& m)
	{
		this->value[0] = m[0];
		this->value[1] = m[1];
		this->value[2] = m[2];
		this->value[3] = m[3];
		return *this;
	}
	extern inline constexpr bool operator==(const Matrix4x4& m1, const Matrix4x4& m2)
	{
		return (m1[0] == m2[0]) && (m1[1] == m2[1]) && (m1[2] == m2[2]) && (m1[3] == m2[3]);
	}
	extern inline constexpr bool operator!=(const Matrix4x4& m1, const Matrix4x4& m2)
	{
		return (m1[0] != m2[0]) || (m1[1] != m2[1]) || (m1[2] != m2[2]) || (m1[3] != m2[3]);
	}

    extern inline constexpr Matrix4x4 mat4Mul(const Matrix4x4& m, const float scalar)
	{
		Vector4 v0 = vec4Mul(&m[0], scalar);
		Vector4 v1 = vec4Mul(&m[1], scalar);
		Vector4 v2 = vec4Mul(&m[2], scalar);
		Vector4 v3 = vec4Mul(&m[3], scalar);

		return Matrix4x4(&v0, &v1, &v2, &v3);
	}
	extern inline constexpr Vector4 mat4Mul(const Matrix4x4& m, const Vector4* v)
	{
		return Vector4{
			m[0][0] * v->x + m[1][0] * v->y + m[2][0] * v->z + m[3][0] * v->w,
			m[0][1] * v->x + m[1][1] * v->y + m[2][1] * v->z + m[3][1] * v->w,
			m[0][2] * v->x + m[1][2] * v->y + m[2][2] * v->z + m[3][2] * v->w,
			m[0][3] * v->x + m[1][3] * v->y + m[2][3] * v->z + m[3][3] * v->w
		};
	}
	extern inline constexpr Vector4 mat4Mul(const Vector4* v, const Matrix4x4& m)
	{
		return Vector4{
			m[0][0] * v->x + m[0][1] * v->y + m[0][2] * v->z + m[0][3] * v->w,
			m[1][0] * v->x + m[1][1] * v->y + m[1][2] * v->z + m[1][3] * v->w,
			m[2][0] * v->x + m[2][1] * v->y + m[2][2] * v->z + m[2][3] * v->w,
			m[3][0] * v->x + m[3][1] * v->y + m[3][2] * v->z + m[3][3] * v->w
		};
	}
    extern inline constexpr Matrix4x4 mat4Mul(const Matrix4x4& m1, const Matrix4x4& m2)
	{
		return Matrix4x4{
			m1[0][0] * m2[0][0] + m1[0][1] * m2[1][0] + m1[0][2] * m2[2][0] + m1[0][3] * m2[3][0],
			m1[0][0] * m2[0][1] + m1[0][1] * m2[1][1] + m1[0][2] * m2[2][1] + m1[0][3] * m2[3][1],
			m1[0][0] * m2[0][2] + m1[0][1] * m2[1][2] + m1[0][2] * m2[2][2] + m1[0][3] * m2[3][2],
			m1[0][0] * m2[0][3] + m1[0][1] * m2[1][3] + m1[0][2] * m2[2][3] + m1[0][3] * m2[3][3],
			m1[1][0] * m2[0][0] + m1[1][1] * m2[1][0] + m1[1][2] * m2[2][0] + m1[1][3] * m2[3][0],
			m1[1][0] * m2[0][1] + m1[1][1] * m2[1][1] + m1[1][2] * m2[2][1] + m1[1][3] * m2[3][1],
			m1[1][0] * m2[0][2] + m1[1][1] * m2[1][2] + m1[1][2] * m2[2][2] + m1[1][3] * m2[3][2],
			m1[1][0] * m2[0][3] + m1[1][1] * m2[1][3] + m1[1][2] * m2[2][3] + m1[1][3] * m2[3][3],
			m1[2][0] * m2[0][0] + m1[2][1] * m2[1][0] + m1[2][2] * m2[2][0] + m1[2][3] * m2[3][0],
			m1[2][0] * m2[0][1] + m1[2][1] * m2[1][1] + m1[2][2] * m2[2][1] + m1[2][3] * m2[3][1],
			m1[2][0] * m2[0][2] + m1[2][1] * m2[1][2] + m1[2][2] * m2[2][2] + m1[2][3] * m2[3][2],
			m1[2][0] * m2[0][3] + m1[2][1] * m2[1][3] + m1[2][2] * m2[2][3] + m1[2][3] * m2[3][3],
			m1[3][0] * m2[0][0] + m1[3][1] * m2[1][0] + m1[3][2] * m2[2][0] + m1[3][3] * m2[3][0],
			m1[3][0] * m2[0][1] + m1[3][1] * m2[1][1] + m1[3][2] * m2[2][1] + m1[3][3] * m2[3][1],
			m1[3][0] * m2[0][2] + m1[3][1] * m2[1][2] + m1[3][2] * m2[2][2] + m1[3][3] * m2[3][2],
			m1[3][0] * m2[0][3] + m1[3][1] * m2[1][3] + m1[3][2] * m2[2][3] + m1[3][3] * m2[3][3]
		};
	}

	extern inline constexpr Matrix4x4 mat4Inverse(const Matrix4x4& m)
	{
		const float a2323 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
		const float a1323 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
		const float a1223 = m[2][1] * m[3][2] - m[2][2] * m[3][1];
		const float a0323 = m[2][0] * m[3][3] - m[2][3] * m[3][0];
		const float a0223 = m[2][0] * m[3][2] - m[2][2] * m[3][0];
		const float a0123 = m[2][0] * m[3][1] - m[2][1] * m[3][0];
		const float a2313 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
		const float a1313 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
		const float a1213 = m[1][1] * m[3][2] - m[1][2] * m[3][1];
		const float a2312 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
		const float a1312 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
		const float a1212 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
		const float a0313 = m[1][0] * m[3][3] - m[1][3] * m[3][0];
		const float a0213 = m[1][0] * m[3][2] - m[1][2] * m[3][0];
		const float a0312 = m[1][0] * m[2][3] - m[1][3] * m[2][0];
		const float a0212 = m[1][0] * m[2][2] - m[1][2] * m[2][0];
		const float a0113 = m[1][0] * m[3][1] - m[1][1] * m[3][0];
		const float a0112 = m[1][0] * m[2][1] - m[1][1] * m[2][0];

		const float determinant = (
			+ m[0][0] * (m[1][1] * a2323 - m[1][2] * a1323 + m[1][3] * a1223)
			- m[0][1] * (m[1][0] * a2323 - m[1][2] * a0323 + m[1][3] * a0223)
			+ m[0][2] * (m[1][0] * a1323 - m[1][1] * a0323 + m[1][3] * a0123)
			- m[0][3] * (m[1][0] * a1223 - m[1][1] * a0223 + m[1][2] * a0123)
			);
		const float oneOverDeterminant = 1 / determinant;

		return Matrix4x4{
			oneOverDeterminant * +(m[1][1] * a2323 - m[1][2] * a1323 + m[1][3] * a1223),
			oneOverDeterminant * -(m[0][1] * a2323 - m[0][2] * a1323 + m[0][3] * a1223),
			oneOverDeterminant * +(m[0][1] * a2313 - m[0][2] * a1313 + m[0][3] * a1213),
			oneOverDeterminant * -(m[0][1] * a2312 - m[0][2] * a1312 + m[0][3] * a1212),
			oneOverDeterminant * -(m[1][0] * a2323 - m[1][2] * a0323 + m[1][3] * a0223),
			oneOverDeterminant * +(m[0][0] * a2323 - m[0][2] * a0323 + m[0][3] * a0223),
			oneOverDeterminant * -(m[0][0] * a2313 - m[0][2] * a0313 + m[0][3] * a0213),
			oneOverDeterminant * +(m[0][0] * a2312 - m[0][2] * a0312 + m[0][3] * a0212),
			oneOverDeterminant * +(m[1][0] * a1323 - m[1][1] * a0323 + m[1][3] * a0123),
			oneOverDeterminant * -(m[0][0] * a1323 - m[0][1] * a0323 + m[0][3] * a0123),
			oneOverDeterminant * +(m[0][0] * a1313 - m[0][1] * a0313 + m[0][3] * a0113),
			oneOverDeterminant * -(m[0][0] * a1312 - m[0][1] * a0312 + m[0][3] * a0112),
			oneOverDeterminant * -(m[1][0] * a1223 - m[1][1] * a0223 + m[1][2] * a0123),
			oneOverDeterminant * +(m[0][0] * a1223 - m[0][1] * a0223 + m[0][2] * a0123),
			oneOverDeterminant * -(m[0][0] * a1213 - m[0][1] * a0213 + m[0][2] * a0113),
			oneOverDeterminant * +(m[0][0] * a1212 - m[0][1] * a0212 + m[0][2] * a0112)
		};
	}
}
