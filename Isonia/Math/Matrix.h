#pragma once

#include "Vector.h"

namespace Isonia::Math
{
	template <uint32_t rows, uint32_t cols> struct Matrix;

	typedef Matrix<3, 3> Matrix3x3;
	typedef Matrix<4, 4> Matrix4x4;

	template<>
	struct Matrix<3, 3>
	{
	public:
		static constexpr uint32_t length() { return 3; }

		constexpr Vector3& operator[](uint32_t i) noexcept;
		constexpr const Vector3& operator[](uint32_t i) const noexcept;

		constexpr Matrix();
		constexpr Matrix(const Matrix3x3& m);
		constexpr Matrix(const float s);
		constexpr Matrix(
			const float x0, const float y0, const float z0,
			const float x1, const float y1, const float z1,
			const float x2, const float y2, const float z2
		);
		constexpr Matrix(const Vector3& v0, const Vector3& v1, const Vector3& v2);
		constexpr Matrix(const Matrix4x4& m);

		constexpr Matrix3x3& operator=(const Matrix3x3& m);
		constexpr Matrix3x3& operator+=(const float s);
		constexpr Matrix3x3& operator+=(const Matrix3x3& m);
		constexpr Matrix3x3& operator-=(const float s);
		constexpr Matrix3x3& operator-=(const Matrix3x3& m);
		constexpr Matrix3x3& operator*=(const float s);
		constexpr Matrix3x3& operator*=(const Matrix3x3& m);
		constexpr Matrix3x3& operator/=(const float s);
		constexpr Matrix3x3& operator/=(const Matrix3x3& m);

		constexpr Matrix3x3& operator++();
		constexpr Matrix3x3& operator--();
		constexpr Matrix3x3 operator++(int);
		constexpr Matrix3x3 operator--(int);

		constexpr Matrix3x3 operator+(const Matrix3x3& m);
		constexpr Matrix3x3 operator-(const Matrix3x3& m);

	private:
		Vector3 value[3];
	};

	constexpr Matrix3x3 operator+(const Matrix3x3& m, const float scalar);
	constexpr Matrix3x3 operator+(const float scalar, const Matrix3x3& m);
	constexpr Matrix3x3 operator+(const Matrix3x3& m1, const Matrix3x3& m2);
	constexpr Matrix3x3 operator-(const Matrix3x3& m, const float scalar);
	constexpr Matrix3x3 operator-(const float scalar, const Matrix3x3& m);
	constexpr Matrix3x3 operator-(const Matrix3x3& m1, const Matrix3x3& m2);
	constexpr Matrix3x3 operator*(const Matrix3x3& m, const float scalar);
	constexpr Matrix3x3 operator*(const float scalar, const Matrix3x3& m);
	constexpr Vector3 operator*(const Matrix3x3& m, const Vector3& v);
	constexpr Vector3 operator*(const Vector3& v, const Matrix3x3& m);
	constexpr Matrix3x3 operator*(const Matrix3x3& m1, const Matrix3x3& m2);
	constexpr Matrix3x3 operator/(const Matrix3x3& m, const float scalar);
	constexpr Matrix3x3 operator/(const float scalar, const Matrix3x3& m);
	constexpr Vector3 operator/(const Matrix3x3& m, const Vector3& v);
	constexpr Vector3 operator/(const Vector3& v, const Matrix3x3& m);
	constexpr Matrix3x3 operator/(const Matrix3x3& m1, const Matrix3x3& m2);

	constexpr bool operator==(const Matrix3x3& m1, const Matrix3x3& m2);
	constexpr bool operator!=(const Matrix3x3& m1, const Matrix3x3& m2);
	
	template <>
	struct Matrix<4, 4>
	{
	public:
		static constexpr uint32_t length() { return 4; }

		constexpr Vector4& operator[](uint32_t i) noexcept;
		constexpr const Vector4& operator[](uint32_t i) const noexcept;

		constexpr Matrix();
		constexpr Matrix(const Matrix4x4& m);
		constexpr Matrix(const float s);
		constexpr Matrix(
			const float x0, const float y0, const float z0, const float w0,
			const float x1, const float y1, const float z1, const float w1,
			const float x2, const float y2, const float z2, const float w2,
			const float x3, const float y3, const float z3, const float w3
		);
		constexpr Matrix(const Vector4& v0, const Vector4& v1, const Vector4& v2, const Vector4& v3);
		constexpr Matrix(const Matrix3x3& m);

		constexpr Matrix4x4& operator=(const Matrix4x4& m);
		constexpr Matrix4x4& operator+=(const float s);
		constexpr Matrix4x4& operator+=(const Matrix4x4& m);
		constexpr Matrix4x4& operator-=(const float s);
		constexpr Matrix4x4& operator-=(const Matrix4x4& m);
		constexpr Matrix4x4& operator*=(const float s);
		constexpr Matrix4x4& operator*=(const Matrix4x4& m);
		constexpr Matrix4x4& operator/=(const float s);
		constexpr Matrix4x4& operator/=(const Matrix4x4& m);

		constexpr Matrix4x4& operator++();
		constexpr Matrix4x4& operator--();
		constexpr Matrix4x4 operator++(int);
		constexpr Matrix4x4 operator--(int);

		constexpr Matrix4x4 operator+(const Matrix4x4& m);
		constexpr Matrix4x4 operator-(const Matrix4x4& m);

	private:
		Vector4 value[4];
	};

	constexpr Matrix4x4 operator+(const Matrix4x4& m, const float scalar);
	constexpr Matrix4x4 operator+(const float scalar, const Matrix4x4& m);
	constexpr Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2);
	constexpr Matrix4x4 operator-(const Matrix4x4& m, const float scalar);
	constexpr Matrix4x4 operator-(const float scalar, const Matrix4x4& m);
	constexpr Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2);
	constexpr Matrix4x4 operator*(const Matrix4x4& m, const float scalar);
	constexpr Matrix4x4 operator*(const float scalar, const Matrix4x4& m);
	constexpr Vector4 operator*(const Matrix4x4& m, const Vector4& v);
	constexpr Vector4 operator*(const Vector4& v, const Matrix4x4& m);
	constexpr Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
	constexpr Matrix4x4 operator/(const Matrix4x4& m, const float scalar);
	constexpr Matrix4x4 operator/(const float scalar, const Matrix4x4& m);
	constexpr Vector4 operator/(const Matrix4x4& m, const Vector4& v);
	constexpr Vector4 operator/(const Vector4& v, const Matrix4x4& m);
	constexpr Matrix4x4 operator/(const Matrix4x4& m1, const Matrix4x4& m2);

	constexpr bool operator==(const Matrix4x4& m1, const Matrix4x4& m2);
	constexpr bool operator!=(const Matrix4x4& m1, const Matrix4x4& m2);


	static constexpr Matrix3x3 Inverse(Matrix3x3 const& m)
	{
		float determinant = (
			+m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
			- m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
			+ m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2])
			);
		float oneOverDeterminant = 1.0f / determinant;

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

	static constexpr Matrix4x4 Inverse(Matrix4x4 const& m)
	{
		float a2323 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
		float a1323 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
		float a1223 = m[2][1] * m[3][2] - m[2][2] * m[3][1];
		float a0323 = m[2][0] * m[3][3] - m[2][3] * m[3][0];
		float a0223 = m[2][0] * m[3][2] - m[2][2] * m[3][0];
		float a0123 = m[2][0] * m[3][1] - m[2][1] * m[3][0];
		float a2313 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
		float a1313 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
		float a1213 = m[1][1] * m[3][2] - m[1][2] * m[3][1];
		float a2312 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
		float a1312 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
		float a1212 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
		float a0313 = m[1][0] * m[3][3] - m[1][3] * m[3][0];
		float a0213 = m[1][0] * m[3][2] - m[1][2] * m[3][0];
		float a0312 = m[1][0] * m[2][3] - m[1][3] * m[2][0];
		float a0212 = m[1][0] * m[2][2] - m[1][2] * m[2][0];
		float a0113 = m[1][0] * m[3][1] - m[1][1] * m[3][0];
		float a0112 = m[1][0] * m[2][1] - m[1][1] * m[2][0];

		float determinant = (
			+m[0][0] * (m[1][1] * a2323 - m[1][2] * a1323 + m[1][3] * a1223)
			- m[0][1] * (m[1][0] * a2323 - m[1][2] * a0323 + m[1][3] * a0223)
			+ m[0][2] * (m[1][0] * a1323 - m[1][1] * a0323 + m[1][3] * a0123)
			- m[0][3] * (m[1][0] * a1223 - m[1][1] * a0223 + m[1][2] * a0123)
			);
		float oneOverDeterminant = 1 / determinant;

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
