#pragma once

#include "Matrix.h"

namespace Isonia::Math
{
	constexpr Vector3& Matrix3x3::operator[](uint32_t i) noexcept
	{
		return this->value[i];
	}
	constexpr const Vector3& Matrix3x3::operator[](uint32_t i) const noexcept
	{
		return this->value[i];
	}

	constexpr Matrix3x3::Matrix() : value{ Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1) } { }
	constexpr Matrix3x3::Matrix(const Matrix3x3& m) : value{ Vector3(m[0]), Vector3(m[1]), Vector3(m[2]) } { }
	constexpr Matrix3x3::Matrix(const float s) : value{ Vector3(s, 0, 0), Vector3(0, s, 0), Vector3(0, 0, s) } { }
	constexpr Matrix3x3::Matrix(
		const float x0, const float y0, const float z0,
		const float x1, const float y1, const float z1,
		const float x2, const float y2, const float z2
	) : value{ Vector3(x0, y0, z0), Vector3(x1, y1, z1), Vector3(x2, y2, z2) } { }
	constexpr Matrix3x3::Matrix(const Vector3& v0, const Vector3& v1, const Vector3& v2) : value{ Vector3(v0), Vector3(v1), Vector3(v2) } { }
	constexpr Matrix3x3::Matrix(const Matrix4x4& m) : value{ Vector3(m[0]), Vector3(m[1]), Vector3(m[2]) } { }

	constexpr Matrix3x3& Matrix3x3::operator=(const Matrix3x3& m)
	{
		this->value[0] = m[0];
		this->value[1] = m[1];
		this->value[2] = m[2];
		return *this;
	}
	constexpr Matrix3x3& Matrix3x3::operator+=(const float s)
	{
		this->value[0] += s;
		this->value[1] += s;
		this->value[2] += s;
		return *this;
	}
	constexpr Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& m)
	{
		this->value[0] += m[0];
		this->value[1] += m[1];
		this->value[2] += m[2];
		return *this;
	}
	constexpr Matrix3x3& Matrix3x3::operator-=(const float s)
	{
		this->value[0] -= s;
		this->value[1] -= s;
		this->value[2] -= s;
		return *this;
	}
	constexpr Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& m)
	{
		this->value[0] -= m[0];
		this->value[1] -= m[1];
		this->value[2] -= m[2];
		return *this;
	}
	constexpr Matrix3x3& Matrix3x3::operator*=(const float s)
	{
		this->value[0] *= s;
		this->value[1] *= s;
		this->value[2] *= s;
		return *this;
	}
	constexpr Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& m)
	{
		return (*this = *this * m);
	}
	constexpr Matrix3x3& Matrix3x3::operator/=(const float s)
	{
		this->value[0] /= s;
		this->value[1] /= s;
		this->value[2] /= s;
		return *this;
	}
	constexpr Matrix3x3& Matrix3x3::operator/=(const Matrix3x3& m)
	{
		return *this *= Inverse(m);
	}

	constexpr Matrix3x3& Matrix3x3::operator++()
	{
		++this->value[0];
		++this->value[1];
		++this->value[2];
		return *this;
	}
	constexpr Matrix3x3& Matrix3x3::operator--()
	{
		--this->value[0];
		--this->value[1];
		--this->value[2];
		return *this;
	}
	constexpr Matrix3x3 Matrix3x3::operator++(int)
	{
		Matrix3x3 result(*this);
		++*this;
		return result;
	}
	constexpr Matrix3x3 Matrix3x3::operator--(int)
	{
		Matrix3x3 result(*this);
		--*this;
		return result;
	}

	constexpr Matrix3x3 Matrix3x3::operator+(const Matrix3x3& m)
	{
		return m;
	}
	constexpr Matrix3x3 Matrix3x3::operator-(const Matrix3x3& m)
	{
		return Matrix3x3(-m[0], -m[1], -m[2]);
	}

	constexpr Matrix3x3 operator+(const Matrix3x3& m, const float scalar)
	{
		return Matrix3x3(m[0] + scalar, m[1] + scalar, m[2] + scalar);
	}
	constexpr Matrix3x3 operator+(const float scalar, const Matrix3x3& m)
	{
		return Matrix3x3(m[0] + scalar, m[1] + scalar, m[2] + scalar);
	}
	constexpr Matrix3x3 operator+(const Matrix3x3& m1, const Matrix3x3& m2)
	{
		return Matrix3x3(m1[0] + m2[0], m1[1] + m2[1], m1[2] + m2[2]);
	}
	constexpr Matrix3x3 operator-(const Matrix3x3& m, const float scalar)
	{
		return Matrix3x3(m[0] - scalar, m[1] - scalar, m[2] - scalar);
	}
	constexpr Matrix3x3 operator-(const float scalar, const Matrix3x3& m)
	{
		return Matrix3x3(scalar - m[0], scalar - m[1], scalar - m[2]);
	}
	constexpr Matrix3x3 operator-(const Matrix3x3& m1, const Matrix3x3& m2)
	{
		return Matrix3x3(m1[0] - m2[0], m1[1] - m2[1], m1[2] - m2[2]);
	}
	constexpr Matrix3x3 operator*(const Matrix3x3& m, const float scalar)
	{
		return Matrix3x3(m[0] * scalar, m[1] * scalar, m[2] * scalar);
	}
	constexpr Matrix3x3 operator*(const float scalar, const Matrix3x3& m)
	{
		return Matrix3x3(m[0] * scalar, m[1] * scalar, m[2] * scalar);
	}
	constexpr Vector3 operator*(const Matrix3x3& m, const Vector3& v)
	{
		return Vector3(
			m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
			m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
			m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z
		);
	}
	constexpr Vector3 operator*(const Vector3& v, const Matrix3x3& m)
	{
		return Vector3(
			m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
			m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
			m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
		);
	}
	constexpr Matrix3x3 operator*(const Matrix3x3& m1, const Matrix3x3& m2)
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
	constexpr Matrix3x3 operator/(const Matrix3x3& m, const float scalar)
	{
		return Matrix3x3(m[0] / scalar, m[1] / scalar, m[2] / scalar);
	}
	constexpr Matrix3x3 operator/(const float scalar, const Matrix3x3& m)
	{
		return Matrix3x3(scalar / m[0], scalar / m[1], scalar / m[2]);
	}
	constexpr Vector3 operator/(const Matrix3x3& m, const Vector3& v)
	{
		return Inverse(m) * v;
	}
	constexpr Vector3 operator/(const Vector3& v, const Matrix3x3& m)
	{
		return v * Inverse(m);
	}
	constexpr Matrix3x3 operator/(const Matrix3x3& m1, const Matrix3x3& m2)
	{
		return m1 * Inverse(m2);
	}

	constexpr bool operator==(const Matrix3x3& m1, const Matrix3x3& m2)
	{
		return (m1[0] == m2[0]) && (m1[1] == m2[1]) && (m1[2] == m2[2]);
	}
	constexpr bool operator!=(const Matrix3x3& m1, const Matrix3x3& m2)
	{
		return (m1[0] != m2[0]) || (m1[1] != m2[1]) || (m1[2] != m2[2]);
	}

	constexpr Vector4& Matrix4x4::operator[](uint32_t i) noexcept
	{
		return this->value[i];
	}
	constexpr const Vector4& Matrix4x4::operator[](uint32_t i) const noexcept
	{
		return this->value[i];
	}

	constexpr Matrix4x4::Matrix() : value{ Vector4(1, 0, 0, 0), Vector4(0, 1, 0, 0), Vector4(0, 0, 1, 0), Vector4(0, 0, 0, 1) } { }
	constexpr Matrix4x4::Matrix(const Matrix4x4& m) : value{ Vector4(m[0]), Vector4(m[1]), Vector4(m[2]), Vector4(m[3]) } { }
	constexpr Matrix4x4::Matrix(const float s) : value{ Vector4(s, 0, 0, 0), Vector4(0, s, 0, 0), Vector4(0, 0, s, 0), Vector4(0, 0, 0, s) } { }
	constexpr Matrix4x4::Matrix(
		const float x0, const float y0, const float z0, const float w0,
		const float x1, const float y1, const float z1, const float w1,
		const float x2, const float y2, const float z2, const float w2,
		const float x3, const float y3, const float z3, const float w3
	) : value{ Vector4(x0, y0, z0, w0), Vector4(x1, y1, z1, w1), Vector4(x2, y2, z2, w2) , Vector4(x3, y3, z3, w3) } { }
	constexpr Matrix4x4::Matrix(const Vector4& v0, const Vector4& v1, const Vector4& v2, const Vector4& v3) : value{ Vector4(v0), Vector4(v1), Vector4(v2) , Vector4(v3) } { }
	constexpr Matrix4x4::Matrix(const Matrix3x3& m) : value{ Vector4(m[0], 0), Vector4(m[1], 0), Vector4(m[2], 0), Vector4(0, 0, 0, 1) } { }

	constexpr Matrix4x4& Matrix4x4::operator=(const Matrix4x4& m)
	{
		this->value[0] = m[0];
		this->value[1] = m[1];
		this->value[2] = m[2];
		this->value[3] = m[3];
		return *this;
	}
	constexpr Matrix4x4& Matrix4x4::operator+=(const float s)
	{
		this->value[0] += s;
		this->value[1] += s;
		this->value[2] += s;
		this->value[3] += s;
		return *this;
	}
	constexpr Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& m)
	{
		this->value[0] += m[0];
		this->value[1] += m[1];
		this->value[2] += m[2];
		this->value[3] += m[3];
		return *this;
	}
	constexpr Matrix4x4& Matrix4x4::operator-=(const float s)
	{
		this->value[0] -= s;
		this->value[1] -= s;
		this->value[2] -= s;
		this->value[3] -= s;
		return *this;
	}
	constexpr Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& m)
	{
		this->value[0] -= m[0];
		this->value[1] -= m[1];
		this->value[2] -= m[2];
		this->value[3] -= m[3];
		return *this;
	}
	constexpr Matrix4x4& Matrix4x4::operator*=(const float s)
	{
		this->value[0] *= s;
		this->value[1] *= s;
		this->value[2] *= s;
		this->value[3] *= s;
		return *this;
	}
	constexpr Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& m)
	{
		return (*this = *this * m);
	}
	constexpr Matrix4x4& Matrix4x4::operator/=(const float s)
	{
		this->value[0] /= s;
		this->value[1] /= s;
		this->value[2] /= s;
		this->value[3] /= s;
		return *this;
	}
	constexpr Matrix4x4& Matrix4x4::operator/=(const Matrix4x4& m)
	{
		return *this *= Inverse(m);
	}

	constexpr Matrix4x4& Matrix4x4::operator++()
	{
		++this->value[0];
		++this->value[1];
		++this->value[2];
		++this->value[3];
		return *this;
	}
	constexpr Matrix4x4& Matrix4x4::operator--()
	{
		--this->value[0];
		--this->value[1];
		--this->value[2];
		--this->value[3];
		return *this;
	}
	constexpr Matrix4x4 Matrix4x4::operator++(int)
	{
		Matrix4x4 result(*this);
		++*this;
		return result;
	}
	constexpr Matrix4x4 Matrix4x4::operator--(int)
	{
		Matrix4x4 result(*this);
		--*this;
		return result;
	}

	constexpr Matrix4x4 Matrix4x4::operator+(const Matrix4x4& m)
	{
		return m;
	}
	constexpr Matrix4x4 Matrix4x4::operator-(const Matrix4x4& m)
	{
		return Matrix4x4(-m[0], -m[1], -m[2], -m[3]);
	}

	constexpr Matrix4x4 operator+(const Matrix4x4& m, const float scalar)
	{
		return Matrix4x4(m[0] + scalar, m[1] + scalar, m[2] + scalar, m[3] + scalar);
	}
	constexpr Matrix4x4 operator+(const float scalar, const Matrix4x4& m)
	{
		return Matrix4x4(m[0] + scalar, m[1] + scalar, m[2] + scalar, m[3] + scalar);
	}
	constexpr Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2)
	{
		return Matrix4x4(m1[0] + m2[0], m1[1] + m2[1], m1[2] + m2[2], m1[3] + m2[3]);
	}
	constexpr Matrix4x4 operator-(const Matrix4x4& m, const float scalar)
	{
		return Matrix4x4(m[0] - scalar, m[1] - scalar, m[2] - scalar, m[3] - scalar);
	}
	constexpr Matrix4x4 operator-(const float scalar, const Matrix4x4& m)
	{
		return Matrix4x4(scalar - m[0], scalar - m[1], scalar - m[2], scalar - m[3]);
	}
	constexpr Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2)
	{
		return Matrix4x4(m1[0] - m2[0], m1[1] - m2[1], m1[2] - m2[2], m1[3] - m2[3]);
	}
	constexpr Matrix4x4 operator*(const Matrix4x4& m, const float scalar)
	{
		return Matrix4x4(m[0] * scalar, m[1] * scalar, m[2] * scalar, m[3] * scalar);
	}
	constexpr Matrix4x4 operator*(const float scalar, const Matrix4x4& m)
	{
		return Matrix4x4(m[0] * scalar, m[1] * scalar, m[2] * scalar, m[3] * scalar);
	}
	constexpr Vector4 operator*(const Matrix4x4& m, const Vector4& v)
	{
		return Vector4(m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3] * v[3]);
	}
	constexpr Vector4 operator*(const Vector4& v, const Matrix4x4& m)
	{
		return Vector4(Dot(m[0], v), Dot(m[1], v), Dot(m[2], v), Dot(m[3], v));
	}
	constexpr Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2)
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
	constexpr Matrix4x4 operator/(const Matrix4x4& m, const float scalar)
	{
		return Matrix4x4(m[0] / scalar, m[1] / scalar, m[2] / scalar, m[3] / scalar);
	}
	constexpr Matrix4x4 operator/(const float scalar, const Matrix4x4& m)
	{
		return Matrix4x4(scalar / m[0], scalar / m[1], scalar / m[2], scalar / m[3]);
	}
	constexpr Vector4 operator/(const Matrix4x4& m, const Vector4& v)
	{
		return Inverse(m) * v;
	}
	constexpr Vector4 operator/(const Vector4& v, const Matrix4x4& m)
	{
		return v * Inverse(m);
	}
	constexpr Matrix4x4 operator/(const Matrix4x4& m1, const Matrix4x4& m2)
	{
		return m1 * Inverse(m2);
	}

	constexpr bool operator==(const Matrix4x4& m1, const Matrix4x4& m2)
	{
		return (m1[0] == m2[0]) && (m1[1] == m2[1]) && (m1[2] == m2[2]) && (m1[3] == m2[3]);
	}
	constexpr bool operator!=(const Matrix4x4& m1, const Matrix4x4& m2)
	{
		return (m1[0] != m2[0]) || (m1[1] != m2[1]) || (m1[2] != m2[2]) || (m1[3] != m2[3]);
	}
}
