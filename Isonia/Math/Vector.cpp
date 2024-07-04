// internal
#include "Math.h"

namespace Isonia::Math
{
    // Vector2
    extern inline constexpr bool operator==(const Vector2& v1, const Vector2& v2)
    {
        return (v1.x == v2.x) && (v1.y == v2.y);
    }
    extern inline constexpr bool operator!=(const Vector2& v1, const Vector2& v2)
    {
        return (v1.x != v2.x) || (v1.y != v2.y);
    }

    extern inline constexpr float vec2Dot(const Vector2* v1, const Vector2* v2)
    {
        return v1->x * v2->x + v1->y * v2->y;
    }
    extern inline constexpr Vector2 vec2Add(const Vector2* v1, const Vector2* v2)
    {
        return Vector2{
            v1->x + v2->x,
            v1->y + v2->y
        };
    }
    extern inline constexpr Vector2 vec2Add(const Vector2* v, const float term)
    {
        return Vector2{
            v->x + term,
            v->y + term
        };
    }
    extern inline constexpr Vector2 vec2Sub(const Vector2* v1, const Vector2* v2)
    {
        return Vector2{
            v1->x - v2->x,
            v1->y - v2->y
        };
    }
    extern inline constexpr Vector2 vec2Sub(const Vector2* v, const float term)
    {
        return Vector2{
            v->x - term,
            v->y - term
        };
    }
    extern inline constexpr Vector2 vec2Mul(const Vector2* v1, const Vector2* v2)
    {
        return Vector2{
            v1->x * v2->x,
            v1->y * v2->y
        };
    }
    extern inline constexpr Vector2 vec2Mul(const Vector2* v, const float scalar)
    {
        return Vector2{
            v->x * scalar,
            v->y * scalar
        };
    }
    extern inline constexpr Vector2 vec2Div(const Vector2* v1, const Vector2* v2)
    {
        return Vector2{
            v1->x / v2->x,
            v1->y / v2->y
        };
    }
    extern inline constexpr Vector2 vec2Div(const Vector2* v, const float divisor)
    {
        return Vector2{
            v->x / divisor,
            v->y / divisor
        };
    }
    extern inline constexpr Vector2 vec2Div(const float divisor, const Vector2* v)
    {
        return Vector2{
            divisor / v->x,
            divisor / v->y
        };
    }
    extern inline constexpr Vector2 vec2Normalize(const Vector2* v)
    {
        return vec2Normalize(v->x, v->y);
    }
    extern inline constexpr Vector2 vec2Normalize(const float x, const float y)
    {
        const float magnitude = sqrtf(x * x + y * y);
        return Vector2{
            x / magnitude,
            y / magnitude
        };
    }

    // Vector3
    extern inline constexpr bool operator==(const Vector3& v1, const Vector3& v2)
    {
        return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
    }
    extern inline constexpr bool operator!=(const Vector3& v1, const Vector3& v2)
    {
        return (v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z);
    }

    extern inline constexpr float vec3Dot(const Vector3* v1, const Vector3* v2)
    {
        return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
    }
    extern inline constexpr Vector3 vec3Add(const Vector3* v1, const Vector3* v2)
    {
        return Vector3{
            v1->x + v2->x,
            v1->y + v2->y,
            v1->z + v2->z
        };
    }
    extern inline constexpr Vector3 vec3Add(const Vector3* v, const float term)
    {
        return Vector3{
            v->x + term,
            v->y + term,
            v->z + term
        };
    }
    extern inline constexpr Vector3 vec3Sub(const Vector3* v1, const Vector3* v2)
    {
        return Vector3{
            v1->x - v2->x,
            v1->y - v2->y,
            v1->z - v2->z
        };
    }
    extern inline constexpr Vector3 vec3Sub(const Vector3* v, const float term)
    {
        return Vector3{
            v->x - term,
            v->y - term,
            v->z - term
        };
    }
    extern inline constexpr Vector3 vec3Mul(const Vector3* v1, const Vector3* v2)
    {
        return Vector3{
            v1->x * v2->x,
            v1->y * v2->y,
            v1->z * v2->z
        };
    }
    extern inline constexpr Vector3 vec3Mul(const Vector3* v, const float scalar)
    {
        return Vector3{
            v->x * scalar,
            v->y * scalar,
            v->z * scalar
        };
    }
    extern inline constexpr Vector3 vec3Div(const Vector3* v1, const Vector3* v2)
    {
        return Vector3{
            v1->x / v2->x,
            v1->y / v2->y,
            v1->z / v2->z
        };
    }
    extern inline constexpr Vector3 vec3Div(const Vector3* v, const float divisor)
    {
        return Vector3{
            v->x / divisor,
            v->y / divisor,
            v->z / divisor
        };
    }
    extern inline constexpr Vector3 vec3Div(const float divisor, const Vector3* v)
    {
        return Vector3{
            divisor / v->x,
            divisor / v->y,
            v->z / divisor
        };
    }
    extern inline constexpr Vector3 vec3Normalize(const Vector3* v)
    {
        return vec3Normalize(v->x, v->y, v->z);
    }
    extern inline constexpr Vector3 vec3Normalize(const float x, const float y, const float z)
    {
        const float magnitude = sqrtf(x * x + y * y + z * z);
        return Vector3{
            x / magnitude,
            y / magnitude,
            z / magnitude
        };
    }
    extern inline constexpr Vector3 vec3Cross(const Vector3* v1, const Vector3* v2)
    {
        return Vector3{
            v1->y * v2->z - v1->z * v2->y,
            v1->z * v2->x - v1->x * v2->z,
            v1->x * v2->y - v1->y * v2->x
        };
    }

    // Vector4
    extern inline constexpr bool operator==(const Vector4& v1, const Vector4& v2)
    {
        return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w);
    }
    extern inline constexpr bool operator!=(const Vector4& v1, const Vector4& v2)
    {
        return (v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z) || (v1.w != v2.w);
    }

    extern inline constexpr float vec4Dot(const Vector4* v1, const Vector4* v2)
    {
        return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w * v2->w;
    }
    extern inline constexpr Vector4 vec4Add(const Vector4* v1, const Vector4* v2)
    {
        return Vector4{
            v1->x + v2->x,
            v1->y + v2->y,
            v1->z + v2->z,
            v1->w + v2->w
        };
    }
    extern inline constexpr Vector4 vec4Add(const Vector4* v, const float term)
    {
        return Vector4{
            v->x + term,
            v->y + term,
            v->z + term,
            v->w + term
        };
    }
    extern inline constexpr Vector4 vec4Sub(const Vector4* v1, const Vector4* v2)
    {
        return Vector4{
            v1->x - v2->x,
            v1->y - v2->y,
            v1->z - v2->z,
            v1->w - v2->w
        };
    }
    extern inline constexpr Vector4 vec4Sub(const Vector4* v, const float term)
    {
        return Vector4{
            v->x - term,
            v->y - term,
            v->z - term,
            v->w - term
        };
    }
    extern inline constexpr Vector4 vec4Mul(const Vector4* v1, const Vector4* v2)
    {
        return Vector4{
            v1->x * v2->x,
            v1->y * v2->y,
            v1->z * v2->z,
            v1->w * v2->w
        };
    }
    extern inline constexpr Vector4 vec4Mul(const Vector4* v, const float scalar)
    {
        return Vector4{
            v->x * scalar,
            v->y * scalar,
            v->z * scalar,
            v->w * scalar
        };
    }
    extern inline constexpr Vector4 vec4Div(const Vector4* v1, const Vector4* v2)
    {
        return Vector4{
            v1->x / v2->x,
            v1->y / v2->y,
            v1->z / v2->z,
            v1->w / v2->w
        };
    }
    extern inline constexpr Vector4 vec4Div(const Vector4* v, const float divisor)
    {
        return Vector4{
            v->x / divisor,
            v->y / divisor,
            v->z / divisor,
            v->w / divisor
        };
    }
    extern inline constexpr Vector4 vec4Div(const float divisor, const Vector4* v)
    {
        return Vector4{
            divisor / v->x,
            divisor / v->y,
            divisor / v->z,
            divisor / v->w
        };
    }
    extern inline constexpr Vector4 vec4Normalize(const Vector4* v)
    {
        return vec4Normalize(v->x, v->y, v->z, v->w);
    }
    extern inline constexpr Vector4 vec4Normalize(const float x, const float y, const float z, const float w)
    {
        const float magnitude = sqrtf(x * x + y * y + z * z + w * w);
        return Vector4{
            x / magnitude,
            y / magnitude,
            z / magnitude,
            w / magnitude
        };
    }

    extern inline constexpr void float4Normalize(float* x, float* y, float* z, float* w)
    {
        const float magnitude = sqrtf(*x * *x + *y * *y + *z * *z + *w * *w);
        *x /= magnitude;
        *y /= magnitude;
        *z /= magnitude;
        *w /= magnitude;
    }
    extern inline constexpr void float3Normalize(float* x, float* y, float* z)
    {
        const float magnitude = sqrtf(*x * *x + *y * *y + *z * *z);
        *x /= magnitude;
        *y /= magnitude;
        *z /= magnitude;
    }
    extern inline constexpr void float2Normalize(float* x, float* y)
    {
        const float magnitude = sqrtf(*x * *x + *y * *y);
        *x /= magnitude;
        *y /= magnitude;
    }
}
