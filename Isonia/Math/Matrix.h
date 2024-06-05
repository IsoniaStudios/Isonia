#pragma once

#include "Vector.h"

namespace Isonia::Math
{
	template <uint32_t rows, uint32_t cols> struct Matrix;

	typedef Matrix<3, 3> Matrix3x3;
	typedef Matrix<4, 4> Matrix4x4;

	static inline Matrix3x3 Inverse(Matrix3x3 const& m)
	{
		float determinant = (
			+ m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
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

	static inline Matrix4x4 Inverse(Matrix4x4 const& m)
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
			+ m[0][0] * (m[1][1] * a2323 - m[1][2] * a1323 + m[1][3] * a1223)
			- m[0][1] * (m[1][0] * a2323 - m[1][2] * a0323 + m[1][3] * a0223)
			+ m[0][2] * (m[1][0] * a1323 - m[1][1] * a0323 + m[1][3] * a0123)
			- m[0][3] * (m[1][0] * a1223 - m[1][1] * a0223 + m[1][2] * a0123)
			);
		float oneOverDeterminant = 1 / determinant;

		return Matrix4x4{
			oneOverDeterminant * (m[1][1] * a2323 - m[1][2] * a1323 + m[1][3] * a1223),
			oneOverDeterminant * -(m[0][1] * a2323 - m[0][2] * a1323 + m[0][3] * a1223),
			oneOverDeterminant * (m[0][1] * a2313 - m[0][2] * a1313 + m[0][3] * a1213),
			oneOverDeterminant * -(m[0][1] * a2312 - m[0][2] * a1312 + m[0][3] * a1212),
			oneOverDeterminant * -(m[1][0] * a2323 - m[1][2] * a0323 + m[1][3] * a0223),
			oneOverDeterminant * (m[0][0] * a2323 - m[0][2] * a0323 + m[0][3] * a0223),
			oneOverDeterminant * -(m[0][0] * a2313 - m[0][2] * a0313 + m[0][3] * a0213),
			oneOverDeterminant * (m[0][0] * a2312 - m[0][2] * a0312 + m[0][3] * a0212),
			oneOverDeterminant * (m[1][0] * a1323 - m[1][1] * a0323 + m[1][3] * a0123),
			oneOverDeterminant * -(m[0][0] * a1323 - m[0][1] * a0323 + m[0][3] * a0123),
			oneOverDeterminant * (m[0][0] * a1313 - m[0][1] * a0313 + m[0][3] * a0113),
			oneOverDeterminant * -(m[0][0] * a1312 - m[0][1] * a0312 + m[0][3] * a0112),
			oneOverDeterminant * -(m[1][0] * a1223 - m[1][1] * a0223 + m[1][2] * a0123),
			oneOverDeterminant * (m[0][0] * a1223 - m[0][1] * a0223 + m[0][2] * a0123),
			oneOverDeterminant * -(m[0][0] * a1213 - m[0][1] * a0213 + m[0][2] * a0113),
			oneOverDeterminant * (m[0][0] * a1212 - m[0][1] * a0212 + m[0][2] * a0112)
		};
	}

	template <>
	struct Matrix<3, 3>
	{
	public:
		static constexpr uint32_t length() { return 3; }

		constexpr Vector3& operator[](uint32_t i) noexcept
		{
			assert(i < this->length());
			return this->value[i];
		}
		constexpr const Vector3& operator[](uint32_t i) const noexcept
		{
			assert(i < this->length());
			return this->value[i];
		}

		constexpr Matrix() : Matrix(1.0f) { }
		constexpr Matrix(const Matrix3x3& m) : value{ Vector3(m[0]), Vector3(m[1]), Vector3(m[2]) } { }
		constexpr Matrix(const float s) : value{ Vector3(s, 0, 0), Vector3(0, s, 0), Vector3(0, 0, s) } { }
		constexpr Matrix(
			const float x0, const float y0, const float z0,
			const float x1, const float y1, const float z1,
			const float x2, const float y2, const float z2
		) : value{ Vector3{x0, y0, z0}, Vector3{x1, y1, z1}, Vector3{x2, y2, z2} } { }
		constexpr Matrix(const Vector3& v0, const Vector3& v1, const Vector3& v2) : value{ Vector3(v0), Vector3(v1), Vector3(v2) } { }
		constexpr Matrix(const Matrix<4, 4>& m) : value{ Vector3(m[0]), Vector3(m[1]), Vector3(m[2]) } { }

		constexpr Matrix3x3& operator=(const Matrix3x3& m)
		{
			this->value[0] = m[0];
			this->value[1] = m[1];
			this->value[2] = m[2];
			return *this;
		}
		constexpr Matrix3x3& operator+=(const float s)
		{
			this->value[0] += s;
			this->value[1] += s;
			this->value[2] += s;
			return *this;
		}
		constexpr Matrix3x3& operator+=(const Matrix3x3& m)
		{
			this->value[0] += m[0];
			this->value[1] += m[1];
			this->value[2] += m[2];
			return *this;
		}
		constexpr Matrix3x3& operator-=(const float s)
		{
			this->value[0] -= s;
			this->value[1] -= s;
			this->value[2] -= s;
			return *this;
		}
		constexpr Matrix3x3& operator-=(const Matrix3x3& m)
		{
			this->value[0] -= m[0];
			this->value[1] -= m[1];
			this->value[2] -= m[2];
			return *this;
		}
		constexpr Matrix3x3& operator*=(const float s)
		{
			this->value[0] *= s;
			this->value[1] *= s;
			this->value[2] *= s;
			return *this;
		}
		constexpr Matrix3x3& operator*=(const Matrix3x3& m)
		{
			return (*this = *this * m);
		}
		constexpr Matrix3x3& operator/=(const float s)
		{
			this->value[0] /= s;
			this->value[1] /= s;
			this->value[2] /= s;
			return *this;
		}
		constexpr Matrix3x3& operator/=(const Matrix3x3& m)
		{
			return *this *= Inverse(m);
		}

		constexpr Matrix3x3& operator++()
		{
			++this->value[0];
			++this->value[1];
			++this->value[2];
			return *this;
		}
		constexpr Matrix3x3& operator--()
		{
			--this->value[0];
			--this->value[1];
			--this->value[2];
			return *this;
		}
		constexpr Matrix3x3 operator++(int)
		{
			Matrix3x3 result(*this);
			++*this;
			return result;
		}
		constexpr Matrix3x3 operator--(int)
		{
			Matrix3x3 result(*this);
			--*this;
			return result;
		}

		constexpr Matrix3x3 operator+(const Matrix3x3& m)
		{
			return m;
		}
		constexpr Matrix3x3 operator-(const Matrix3x3& m)
		{
			return Matrix3x3(-m[0], -m[1], -m[2]);
		}

	private:
		Vector3 value[3];
	};

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
		float a00 = m1[0][0];
		float a01 = m1[0][1];
		float a02 = m1[0][2];
		float a10 = m1[1][0];
		float a11 = m1[1][1];
		float a12 = m1[1][2];
		float a20 = m1[2][0];
		float a21 = m1[2][1];
		float a22 = m1[2][2];

		float b00 = m2[0][0];
		float b01 = m2[0][1];
		float b02 = m2[0][2];
		float b10 = m2[1][0];
		float b11 = m2[1][1];
		float b12 = m2[1][2];
		float b20 = m2[2][0];
		float b21 = m2[2][1];
		float b22 = m2[2][2];

		return Matrix3x3{
			a00 * b00 + a10 * b01 + a20 * b02,
			a01 * b00 + a11 * b01 + a21 * b02,
			a02 * b00 + a12 * b01 + a22 * b02,
			a00 * b10 + a10 * b11 + a20 * b12,
			a01 * b10 + a11 * b11 + a21 * b12,
			a02 * b10 + a12 * b11 + a22 * b12,
			a00 * b20 + a10 * b21 + a20 * b22,
			a01 * b20 + a11 * b21 + a21 * b22,
			a02 * b20 + a12 * b21 + a22 * b22
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

	template <>
	struct Matrix<4, 4>
	{
	public:
		static constexpr uint32_t length() { return 4; }

		constexpr Vector4& operator[](uint32_t i) noexcept
		{
			assert(i < this->length());
			return this->value[i];
		}
		constexpr const Vector4& operator[](uint32_t i) const noexcept
		{
			assert(i < this->length());
			return this->value[i];
		}

		constexpr Matrix() : Matrix(1.0f) { }
		constexpr Matrix(const Matrix4x4& m) : value{ Vector4(m[0]), Vector4(m[1]), Vector4(m[2]), Vector4(m[3]) } { }
		constexpr Matrix(const float s) : value{ Vector4(s, 0, 0, 0), Vector4(0, s, 0, 0), Vector4(0, 0, s, 0), Vector4(0, 0, 0, s) } { }
		constexpr Matrix(
			const float x0, const float y0, const float z0, const float w0,
			const float x1, const float y1, const float z1, const float w1,
			const float x2, const float y2, const float z2, const float w2,
			const float x3, const float y3, const float z3, const float w3
		) : value{ Vector4{x0, y0, z0, w0}, Vector4{x1, y1, z1, w1}, Vector4{x2, y2, z2, w2} } { }
		constexpr Matrix(const Vector4& v0, const Vector4& v1, const Vector4& v2, const Vector4& v3) : value{ Vector4(v0), Vector4(v1), Vector4(v2) , Vector4(v3) } { }
		constexpr Matrix(const Matrix<3, 3>& m) : value{ Vector4(m[0], 0), Vector4(m[1], 0), Vector4(m[2], 0), Vector4(0, 0, 0, 1) } { }

		constexpr Matrix4x4& operator=(const Matrix4x4& m)
		{
			this->value[0] = m[0];
			this->value[1] = m[1];
			this->value[2] = m[2];
			this->value[3] = m[3];
			return *this;
		}
		constexpr Matrix4x4& operator+=(const float s)
		{
			this->value[0] += s;
			this->value[1] += s;
			this->value[2] += s;
			this->value[3] += s;
			return *this;
		}
		constexpr Matrix4x4& operator+=(const Matrix4x4& m)
		{
			this->value[0] += m[0];
			this->value[1] += m[1];
			this->value[2] += m[2];
			this->value[3] += m[3];
			return *this;
		}
		constexpr Matrix4x4& operator-=(const float s)
		{
			this->value[0] -= s;
			this->value[1] -= s;
			this->value[2] -= s;
			this->value[3] -= s;
			return *this;
		}
		constexpr Matrix4x4& operator-=(const Matrix4x4& m)
		{
			this->value[0] -= m[0];
			this->value[1] -= m[1];
			this->value[2] -= m[2];
			this->value[3] -= m[3];
			return *this;
		}
		constexpr Matrix4x4& operator*=(const float s)
		{
			this->value[0] *= s;
			this->value[1] *= s;
			this->value[2] *= s;
			this->value[3] *= s;
			return *this;
		}
		constexpr Matrix4x4& operator*=(const Matrix4x4& m)
		{
			return (*this = *this * m);
		}
		constexpr Matrix4x4& operator/=(const float s)
		{
			this->value[0] /= s;
			this->value[1] /= s;
			this->value[2] /= s;
			this->value[3] /= s;
			return *this;
		}
		constexpr Matrix4x4& operator/=(const Matrix4x4& m)
		{
			return *this *= Inverse(m);
		}

		constexpr Matrix4x4& operator++()
		{
			++this->value[0];
			++this->value[1];
			++this->value[2];
			++this->value[3];
			return *this;
		}
		constexpr Matrix4x4& operator--()
		{
			--this->value[0];
			--this->value[1];
			--this->value[2];
			--this->value[3];
			return *this;
		}
		constexpr Matrix4x4 operator++(int)
		{
			Matrix4x4 result(*this);
			++*this;
			return result;
		}
		constexpr Matrix4x4 operator--(int)
		{
			Matrix4x4 result(*this);
			--*this;
			return result;
		}

		constexpr Matrix4x4 operator+(const Matrix4x4& m)
		{
			return m;
		}
		constexpr Matrix4x4 operator-(const Matrix4x4& m)
		{
			return Matrix4x4(-m[0], -m[1], -m[2], -m[3]);
		}

	private:
		Vector4 value[4];
	};

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
		Vector4 mov0(v[0]);
		Vector4 mov1(v[1]);
		Vector4 mul0 = m[0] * mov0;
		Vector4 mul1 = m[1] * mov1;
		Vector4 add0 = mul0 + mul1;

		Vector4 mov2(v[2]);
		Vector4 mov3(v[3]);
		Vector4 mul2 = m[2] * mov2;
		Vector4 mul3 = m[3] * mov3;
		Vector4 add1 = mul2 + mul3;
		return add0 + add1;
	}
	constexpr Vector4 operator*(const Vector4& v, const Matrix4x4& m)
	{
		return Vector4(glm::dot(m[0], v), glm::dot(m[1], v), glm::dot(m[2], v), glm::dot(m[3], v));
	}
	//constexpr Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2)
	//{

	//}
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
