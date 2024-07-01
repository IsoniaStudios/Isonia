#pragma once

// std
#include "cassert"

namespace Isonia::Math
{
    // Constants
    extern inline constexpr const float pi = 3.14159265359f;
    extern inline constexpr const float two_pi = pi * 2.0f;
    
    extern inline constexpr const float eulers_number = 2.718281828459045f;

    extern inline constexpr const float epsilon = 1.192092896e-07f;
    extern inline constexpr const float two_epsilon = epsilon * 2.0f;

    extern inline constexpr const float isometric_y_scale = 1.154700538379252f; //1.0f / Math::Cos(Math::Radians(-30.0f));

    extern inline constexpr const float pixels_per_unit = 16.0f;
    extern inline constexpr const float units_per_pixel = 1.0f / pixels_per_unit;

    extern inline constexpr const float rotation_grid = 30.0f / 16.0f;
    extern inline constexpr const float inverse_rotation_grid = 1.0f / rotation_grid;

    extern inline constexpr const float float_max = 3.402823466e+38F;
    extern inline constexpr const unsigned int unsigned_int_max = 2147483647;
    extern inline constexpr const unsigned long unsigned_long_max = 2147483647L;

    // Generics
    extern inline constexpr float floorf(const float f);
    extern inline constexpr float ceilf(const float f);
    extern inline constexpr float roundf(const float f);
    extern inline constexpr float truncf(const float f);

    extern inline constexpr int absi(const int i);

    extern inline constexpr float absf(const float f);

    extern inline float fmodf(const float x, const float y);

    extern inline constexpr int mini(const int a, const int b);
    extern inline constexpr int maxi(const int a, const int b);

    extern inline constexpr float minf(const float a, const float b);
    extern inline constexpr float maxf(const float a, const float b);
    extern inline constexpr float clampf(const float f, const float min, const float max);
    extern inline constexpr unsigned int clampui(const unsigned int i, const unsigned int min, const unsigned int max);
    extern inline constexpr int clampi(const int i, const int min, const int max);

    extern inline constexpr float sqrtf(const float f);
    extern inline constexpr float rsqrtf(const float f);
    extern inline constexpr float powf(const float f, const float e);
    extern inline constexpr float powPrecisef(const float f, const float e);
    extern inline float powExactf(const float f, const float e);
    
    // Generics To-Int
    extern inline constexpr int ceilf_i(const float f);
    extern inline constexpr int floorf_i(const float f);
    extern inline constexpr int roundf_i(const float f);

    extern inline constexpr int ceilEvenf_i(const float f);
    extern inline constexpr int ceilOddf_i(const float f);

    // Trigonometry
    extern inline constexpr float radiansf(const float degrees);
    extern inline constexpr float degreesf(const float radians);

    extern inline float clampDegreesf(const float degrees);
    extern inline float clampRadiansf(const float radians);

    extern inline float cosf(const float radians);
    extern inline float sinf(const float radians);
    extern inline float tanf(const float radians);
    extern inline float atanf(const float radians);
    extern inline float atan2f(const float y, const float x);

    // Vector
    union Vector2;
    union Vector3;
    union Vector4;
    
    union Vector4
    {
    public:
        static constexpr unsigned int length() { return 4u; }

        constexpr Vector4();
        constexpr Vector4(const Vector2* v, const float z, const float w);
        constexpr Vector4(const Vector3* v, const float w);
        constexpr Vector4(const Vector4* v);
        constexpr Vector4(const float x, const float y, const float z, const float w);
        constexpr Vector4(const float xyzw);

        constexpr float& operator[](const unsigned int i) noexcept { return value[i]; }
        constexpr const float& operator[](const unsigned int i) const noexcept { return value[i]; }

        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
        float value[4];
    };

    extern inline constexpr bool operator==(const Vector4& v1, const Vector4& v2);
    extern inline constexpr bool operator!=(const Vector4& v1, const Vector4& v2);

    extern inline constexpr float vec4Dot(const Vector4* v1, const Vector4* v2);
    extern inline constexpr Vector4 vec4Add(const Vector4* v1, const Vector4* v2);
    extern inline constexpr Vector4 vec4Add(const Vector4* v, const float term);
    extern inline constexpr Vector4 vec4Sub(const Vector4* v1, const Vector4* v2);
    extern inline constexpr Vector4 vec4Sub(const Vector4* v, const float term);
    extern inline constexpr Vector4 vec4Mul(const Vector4* v1, const Vector4* v2);
    extern inline constexpr Vector4 vec4Mul(const Vector4* v, const float scalar);
    extern inline constexpr Vector4 vec4Div(const Vector4* v1, const Vector4* v2);
    extern inline constexpr Vector4 vec4Div(const Vector4* v, const float divisor);
    extern inline constexpr Vector4 vec4Div(const float divisor, const Vector4* v);
    extern inline constexpr Vector4 vec4Normalize(const Vector4* v);
    extern inline constexpr Vector4 vec4Normalize(const float x, const float y, const float z, const float w);

    union Vector3
    {
    public:
        static constexpr unsigned int length() { return 3u; }

        constexpr Vector3();
        constexpr Vector3(const Vector2* v, const float z);
        constexpr Vector3(const Vector3* v);
        constexpr Vector3(const Vector4* v);
        constexpr Vector3(const float x, const float y, const float z);
        constexpr Vector3(const float xyz);

        constexpr float& operator[](const unsigned int i) noexcept { return value[i]; }
        constexpr const float& operator[](const unsigned int i) const noexcept { return value[i]; }

        struct
        {
            float x;
            float y;
            float z;
        };
        float value[3];
    };

    extern inline constexpr bool operator==(const Vector3& v1, const Vector3& v2);
    extern inline constexpr bool operator!=(const Vector3& v1, const Vector3& v2);

    extern inline constexpr float vec3Dot(const Vector3* v1, const Vector3* v2);
    extern inline constexpr Vector3 vec3Add(const Vector3* v1, const Vector3* v2);
    extern inline constexpr Vector3 vec3Add(const Vector3* v, const float term);
    extern inline constexpr Vector3 vec3Sub(const Vector3* v1, const Vector3* v2);
    extern inline constexpr Vector3 vec3Sub(const Vector3* v, const float term);
    extern inline constexpr Vector3 vec3Mul(const Vector3* v1, const Vector3* v2);
    extern inline constexpr Vector3 vec3Mul(const Vector3* v, const float scalar);
    extern inline constexpr Vector3 vec3Div(const Vector3* v1, const Vector3* v2);
    extern inline constexpr Vector3 vec3Div(const Vector3* v, const float divisor);
    extern inline constexpr Vector3 vec3Div(const float divisor, const Vector3* v);
    extern inline constexpr Vector3 vec3Normalize(const Vector3* v);
    extern inline constexpr Vector3 vec3Normalize(const float x, const float y, const float z);
    extern inline constexpr Vector3 vec3Cross(const Vector3* v1, const Vector3* v2);

    union Vector2
    {
    public:
        static constexpr unsigned int length() { return 2u; }

        constexpr Vector2();
        constexpr Vector2(const Vector2* v);
        constexpr Vector2(const Vector3* v);
        constexpr Vector2(const Vector4* v);
        constexpr Vector2(const float x, const float y);
        constexpr Vector2(const float xy);

        constexpr float& operator[](const unsigned int i) noexcept { return value[i]; }
        constexpr const float& operator[](const unsigned int i) const noexcept { return value[i]; }

        struct
        {
            float x;
            float y;
        };
        float value[2];
    };

    extern inline constexpr bool operator==(const Vector2& v1, const Vector2& v2);
    extern inline constexpr bool operator!=(const Vector2& v1, const Vector2& v2);

    extern inline constexpr float vec2Dot(const Vector2* v1, const Vector2* v2);
    extern inline constexpr Vector2 vec2Add(const Vector2* v1, const Vector2* v2);
    extern inline constexpr Vector2 vec2Add(const Vector2* v, const float term);
    extern inline constexpr Vector2 vec2Sub(const Vector2* v1, const Vector2* v2);
    extern inline constexpr Vector2 vec2Sub(const Vector2* v, const float term);
    extern inline constexpr Vector2 vec2Mul(const Vector2* v1, const Vector2* v2);
    extern inline constexpr Vector2 vec2Mul(const Vector2* v, const float scalar);
    extern inline constexpr Vector2 vec2Div(const Vector2* v1, const Vector2* v2);
    extern inline constexpr Vector2 vec2Div(const Vector2* v, const float divisor);
    extern inline constexpr Vector2 vec2Div(const float divisor, const Vector2* v);
    extern inline constexpr Vector2 vec2Normalize(const Vector2* v);
    extern inline constexpr Vector2 vec2Normalize(const float x, const float y);

    constexpr Vector4::Vector4() : x(0), y(0), z(0), w(0) { }
    constexpr Vector4::Vector4(const Vector2* v, const float z, const float w) : x(v->x), y(v->y), z(z), w(w) { }
    constexpr Vector4::Vector4(const Vector3* v, const float w) : x(v->x), y(v->y), z(v->z), w(w) { }
    constexpr Vector4::Vector4(const Vector4* v) : x(v->x), y(v->y), z(v->z), w(v->w) { }
    constexpr Vector4::Vector4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) { }
    constexpr Vector4::Vector4(const float xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) { }

    constexpr Vector3::Vector3() : x(0), y(0), z(0) { }
    constexpr Vector3::Vector3(const Vector2* v, const float z) : x(v->x), y(v->y), z(z) { }
    constexpr Vector3::Vector3(const Vector3* v) : x(v->x), y(v->y), z(v->z) { }
    constexpr Vector3::Vector3(const Vector4* v) : x(v->x), y(v->y), z(v->z) { }
    constexpr Vector3::Vector3(const float x, const float y, const float z) : x(x), y(y), z(z) { }
    constexpr Vector3::Vector3(const float xyz) : x(xyz), y(xyz), z(xyz) { }

    constexpr Vector2::Vector2() : x(0), y(0) { }
    constexpr Vector2::Vector2(const Vector2* v) : x(v->x), y(v->y) { }
    constexpr Vector2::Vector2(const Vector3* v) : x(v->x), y(v->y) { }
    constexpr Vector2::Vector2(const Vector4* v) : x(v->x), y(v->y) { }
    constexpr Vector2::Vector2(const float x, const float y) : x(x), y(y) { }
    constexpr Vector2::Vector2(const float xy) : x(xy), y(xy) { }

    // Matrix
    struct Matrix3x3;
    struct Matrix4x4;

    struct Matrix3x3
    {
    public:
        static constexpr unsigned int length() { return 3u; }

        constexpr Matrix3x3();
        constexpr Matrix3x3(const Matrix3x3& m);
        constexpr Matrix3x3(const float s);
        constexpr Matrix3x3(
            const float x0, const float y0, const float z0,
            const float x1, const float y1, const float z1,
            const float x2, const float y2, const float z2
        );
        constexpr Matrix3x3(const Vector3* v0, const Vector3* v1, const Vector3* v2);
        constexpr Matrix3x3(const Matrix4x4& m);

        constexpr Vector3& operator[](const unsigned int i) noexcept { return value[i]; }
        constexpr const Vector3& operator[](const unsigned int i) const noexcept { return value[i]; }

    private:
        Vector3 value[3];
    };

    extern inline constexpr bool operator==(const Matrix3x3& m1, const Matrix3x3& m2);
    extern inline constexpr bool operator!=(const Matrix3x3& m1, const Matrix3x3& m2);

    extern inline constexpr Matrix3x3 mat3Mul(const Matrix3x3& m, const float scalar);
    extern inline constexpr Vector3 mat3Mul(const Matrix3x3& m, const Vector3* v);
    extern inline constexpr Vector3 mat3Mul(const Vector3* v, const Matrix3x3& m);
    extern inline constexpr Matrix3x3 mat3Mul(const Matrix3x3& m1, const Matrix3x3& m2);
    extern inline constexpr Matrix3x3 mat3Inverse(const Matrix3x3& m);

    struct Matrix4x4
    {
    public:
        static constexpr unsigned int length() { return 4u; }

        constexpr Matrix4x4();
        constexpr Matrix4x4(const Matrix4x4& m);
        constexpr Matrix4x4(const float s);
        constexpr Matrix4x4(
            const float x0, const float y0, const float z0, const float w0,
            const float x1, const float y1, const float z1, const float w1,
            const float x2, const float y2, const float z2, const float w2,
            const float x3, const float y3, const float z3, const float w3
        );
        constexpr Matrix4x4(const Vector4* v0, const Vector4* v1, const Vector4* v2, const Vector4* v3);
        constexpr Matrix4x4(const Matrix3x3& m);

        constexpr Vector4& operator[](const unsigned int i) noexcept { return value[i]; }
        constexpr const Vector4& operator[](const unsigned int i) const noexcept { return value[i]; }

    private:
        Vector4 value[4];
    };

    extern inline constexpr bool operator==(const Matrix4x4& m1, const Matrix4x4& m2);
    extern inline constexpr bool operator!=(const Matrix4x4& m1, const Matrix4x4& m2);

    extern inline constexpr Matrix4x4 mat4Mul(const Matrix4x4& m, const float scalar);
    extern inline constexpr Vector4 mat4Mul(const Matrix4x4& m, const Vector4* v);
    extern inline constexpr Vector4 mat4Mul(const Vector4* v, const Matrix4x4& m);
    extern inline constexpr Matrix4x4 mat4Mul(const Matrix4x4& m1, const Matrix4x4& m2);
    extern inline constexpr Matrix4x4 mat4Inverse(const Matrix4x4& m);

    constexpr Matrix3x3::Matrix3x3() : value{ Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1) } { }
    constexpr Matrix3x3::Matrix3x3(const Matrix3x3& m) : value{ Vector3(m[0]), Vector3(m[1]), Vector3(m[2]) } { }
    constexpr Matrix3x3::Matrix3x3(const float s) : value{ Vector3(s, 0, 0), Vector3(0, s, 0), Vector3(0, 0, s) } { }
    constexpr Matrix3x3::Matrix3x3(
        const float x0, const float y0, const float z0,
        const float x1, const float y1, const float z1,
        const float x2, const float y2, const float z2
    ) : value{ Vector3(x0, y0, z0), Vector3(x1, y1, z1), Vector3(x2, y2, z2) } { }
    constexpr Matrix3x3::Matrix3x3(const Vector3* v0, const Vector3* v1, const Vector3* v2) : value{ Vector3(v0), Vector3(v1), Vector3(v2) } { }
    constexpr Matrix3x3::Matrix3x3(const Matrix4x4& m) : value{ Vector3(&m[0]), Vector3(&m[1]), Vector3(&m[2]) } { }

    constexpr Matrix4x4::Matrix4x4() : value{ Vector4(1, 0, 0, 0), Vector4(0, 1, 0, 0), Vector4(0, 0, 1, 0), Vector4(0, 0, 0, 1) } { }
    constexpr Matrix4x4::Matrix4x4(const Matrix4x4& m) : value{ Vector4(m[0]), Vector4(m[1]), Vector4(m[2]), Vector4(m[3]) } { }
    constexpr Matrix4x4::Matrix4x4(const float s) : value{ Vector4(s, 0, 0, 0), Vector4(0, s, 0, 0), Vector4(0, 0, s, 0), Vector4(0, 0, 0, s) } { }
    constexpr Matrix4x4::Matrix4x4(
        const float x0, const float y0, const float z0, const float w0,
        const float x1, const float y1, const float z1, const float w1,
        const float x2, const float y2, const float z2, const float w2,
        const float x3, const float y3, const float z3, const float w3
    ) : value{ Vector4(x0, y0, z0, w0), Vector4(x1, y1, z1, w1), Vector4(x2, y2, z2, w2) , Vector4(x3, y3, z3, w3) } { }
    constexpr Matrix4x4::Matrix4x4(const Vector4* v0, const Vector4* v1, const Vector4* v2, const Vector4* v3) : value{ Vector4(v0), Vector4(v1), Vector4(v2) , Vector4(v3) } { }
    constexpr Matrix4x4::Matrix4x4(const Matrix3x3& m) : value{ Vector4(&m[0], 0), Vector4(&m[1], 0), Vector4(&m[2], 0), Vector4(0, 0, 0, 1) } { }

    struct Transform
    {
    public:
        Math::Vector3 position;
        Math::Vector3 rotation;
        Math::Vector3 scale;

        Transform();
        Transform(Math::Vector3 initial_position, Math::Vector3 initial_rotation, Math::Vector3 initial_scale);

        const Math::Matrix4x4 matrix4x4() const;
        const Math::Matrix3x3 normalMatrix3x3() const;
    };

    // Retro
    extern inline constexpr Vector3 roundVec3ToPixel(const Vector3 vector);
    extern inline constexpr Vector4 roundVec4ToPixel(const Vector4 vector);

    // Geometry
    extern inline constexpr bool intersectRayPlane(const Vector3* origin, const Vector3* direction, const Vector3* plane_origin, const Vector3* plane_normal, float* intersection_distance);

    // Interpolation
    extern inline constexpr float interpolationHermitef(const float t);
    extern inline constexpr float interpolationQuinticf(const float t);

    extern inline constexpr float lerpf(const float a, const float b, const float t);
    extern inline constexpr Vector3 lerpv3(const Vector3* a, const Vector3* b, const float t);
    extern inline constexpr float cubicLerpf(const float a, const float b, const float c, const float d, const float t);
    extern inline constexpr Vector3 cubicLerpv3(const Vector3* a, const Vector3* b, const Vector3* c, const Vector3* d, const float t);

    extern inline float exponentialDecayf(const float a, const float b, const float t);
    extern inline float sigmoidf(const float a, const float b, const float t);

    extern inline constexpr Vector3 smoothNormalFromFour(const float* height_map, const float dx, const float dz, const unsigned int width, const unsigned int height, const unsigned int z, const unsigned int x);
    extern inline constexpr Vector3 smoothNormalFromEight(const float* height_map, const float dx, const float dz, const unsigned int width, const unsigned int height, const unsigned int z, const unsigned int x);

    // Hash
    static inline constexpr unsigned int hash2D(const unsigned int seed, const unsigned int x_primed, const unsigned int y_primed);
    static inline constexpr unsigned int hash3D(const unsigned int seed, const unsigned int x_primed, const unsigned int y_primed, const unsigned int z_primed);
    static inline constexpr unsigned int hash4D(const unsigned int seed, const unsigned int x_primed, const unsigned int y_primed, const unsigned int z_primed, const unsigned int w_primed);

    extern inline constexpr float valCoord(const unsigned int seed, const unsigned int x_primed, const unsigned int y_primed);
    extern inline constexpr float valCoord(const unsigned int seed, const unsigned int x_primed, const unsigned int y_primed, const unsigned int z_primed);
    extern inline constexpr float valCoord(const unsigned int seed, const unsigned int x_primed, const unsigned int y_primed, const unsigned int z_primed, const unsigned int w_primed);

    extern inline constexpr float gradCoord(const unsigned int seed, const unsigned int x_primed, const unsigned int y_primed, const float xd, const float yd);
    extern inline constexpr float gradCoord(const unsigned int seed, const unsigned int x_primed, const unsigned int y_primed, const unsigned int z_primed, const float xd, const float yd, const float zd);
    extern inline constexpr float gradCoord(const unsigned int seed, const unsigned int x_primed, const unsigned int y_primed, const unsigned int z_primed, const unsigned int w_primed, const float xd, const float yd, const float zd, const float wd);

    // Random
    extern inline constexpr unsigned int randomInt(const unsigned int seed);
}