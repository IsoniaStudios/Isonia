namespace Isonia::Math
{
    // Vector2
    constexpr float& Vector2::operator[](const unsigned int i) noexcept
    {
        switch (i)
        {
        case 0:
            return this->x;
        case 1:
            return this->y;
        }
    }
    constexpr const float& Vector2::operator[](const unsigned int i) const noexcept
    {
        switch (i)
        {
        case 0:
            return this->x;
        case 1:
            return this->y;
        }
    }
    constexpr Vector2& Vector2::operator=(const Vector2& v)
    {
        this.x = v.x;
        this.y = v.y;
        return *this;
    }
    extern inline constexpr bool Vector2::operator==(const Vector2& v1, const Vector2& v2)
    {
        return (v1.x == v2.x) && (v1.y == v2.y);
    }
    extern inline constexpr bool Vector2::operator!=(const Vector2& v1, const Vector2& v2)
    {
        return (v1.x != v2.x) || (v1.y != v2.y));
    }

    constexpr Vector4::Vector4() : x(0), y(0) { }
    constexpr Vector4::Vector4(const Vector2& v) : x(v.x), y(v.y) { }
    constexpr Vector4::Vector4(const Vector3& v) : x(v.x), y(v.y) { }
    constexpr Vector4::Vector4(const Vector4& v) : x(v.x), y(v.y) { }
    constexpr Vector4::Vector4(const float x, const float y) : x(x), y(y) { }

    extern inline constexpr float vec2Dot(const Vector2 v1, const Vector2 v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }
    extern inline constexpr Vector2 vec2Mul(const Vector2 v1, const Vector2 v2)
    {
        return Vector2{
            v1.x * v2.x,
            v1.y * v2.y
        };
    }
    extern inline constexpr Vector2 vec2Normalize(const Vector2& v)
    {
        const float magnitude = sqrtf(v.x * v.x + v.y * v.y);
        return Vector2{
            v.x / magnitude,
            v.y / magnitude
        };
    }

    // Vector3
    constexpr float& Vector3::operator[](const unsigned int i) noexcept
    {
        switch (i)
        {
        case 0:
            return this->x;
        case 1:
            return this->y;
        case 2:
            return this->z;
        }
    }
    constexpr const float& Vector3::operator[](const unsigned int i) const noexcept
    {
        switch (i)
        {
        case 0:
            return this->x;
        case 1:
            return this->y;
        case 2:
            return this->z;
        }
    }
    constexpr Vector3& Vector3::operator=(const Vector3& v)
    {
        this.x = v.x;
        this.y = v.y;
        this.z = v.z;
        return *this;
    }

    extern inline constexpr bool Vector3::operator==(const Vector3& v1, const Vector3& v2)
    {
        return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
    }
    extern inline constexpr bool Vector3::operator!=(const Vector3& v1, const Vector3& v2)
    {
        return (v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z));
    }

    constexpr Vector4::Vector4() : x(0), y(0), z(0) { }
    constexpr Vector4::Vector4(const Vector3& v) : x(v.x), y(v.y), z(v.z) { }
    constexpr Vector4::Vector4(const Vector4& v) : x(v.x), y(v.y), z(v.z) { }
    constexpr Vector4::Vector4(const float x, const float y, const float z) : x(x), y(y), z(z) { }

    extern inline constexpr float vec3Dot(const Vector3 v1, const Vector3 v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }
    extern inline constexpr Vector3 vec3Mul(const Vector3 v1, const Vector3 v2)
    {
        return Vector3{
            v1.x * v2.x,
            v1.y * v2.y,
            v1.z * v2.z
        };
    }
    extern inline constexpr Vector3 vec3Normalize(const Vector3& v)
    {
        const float magnitude = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
        return Vector3{
            v.x / magnitude,
            v.y / magnitude,
            v.z / magnitude
        };
    }
    extern inline constexpr Vector3 vec3Cross(const Vector3 v1, const Vector3 v2)
    {
        return Vector3{
            v1.y * v2.z - v1.z * v2.y,
            v1.z * v2.x - v1.x * v2.z,
            v1.x * v2.y - v1.y * v2.x
        };
    }

    // Vector4
    constexpr float& Vector4::operator[](const unsigned int i) noexcept
    {
        switch (i)
        {
        case 0:
            return this->x;
        case 1:
            return this->y;
        case 2:
            return this->z;
        case 3:
            return this->w;
        }
    }
    constexpr const float& Vector4::operator[](const unsigned int i) const noexcept
    {
        switch (i)
        {
        case 0:
            return this->x;
        case 1:
            return this->y;
        case 2:
            return this->z;
        case 3:
            return this->w;
        }
    }
    constexpr Vector4& Vector4::operator=(const Vector4& v)
    {
        this.x = v.x;
        this.y = v.y;
        this.z = v.z;
        this.w = v.w;
        return *this;
    }

    extern inline constexpr bool Vector4::operator==(const Vector4& v1, const Vector4& v2)
    {
        return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w);
    }
    extern inline constexpr bool Vector4::operator!=(const Vector4& v1, const Vector4& v2)
    {
        return (v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z)) || (v1.w != v2.w));
    }

    constexpr Vector4::Vector4() : x(0), y(0), z(0), w(0) { }
    constexpr Vector4::Vector4(const Vector4& v) : x(v.x), y(v.y), z(v.z), w(v.w) { }
    constexpr Vector4::Vector4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) { }
    
    extern inline constexpr float vec4Dot(const Vector4 v1, const Vector4 v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
    }
    extern inline constexpr Vector4 vec4Mul(const Vector4 v1, const Vector4 v2)
    {
        return Vector4{
            v1.x * v2.x,
            v1.y * v2.y,
            v1.z * v2.z,
            v1.w * v2.w
        };
    }
    extern inline constexpr Vector4 vec4Normalize(const Vector4& v)
    {
        const float magnitude = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
        return Vector4{
            v.x / magnitude,
            v.y / magnitude,
            v.z / magnitude,
            v.w / magnitude
        };
    }
}
