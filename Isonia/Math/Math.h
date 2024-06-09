#ifndef IMATH
#define IMATH

namespace Isonia::Math
{
    // Constants
    extern constexpr const float pi = 3.14159265359f;
    extern constexpr const float two_pi = pi * 2.0f;
    
    extern constexpr const float epsilon = 1.192092896e-07f;

    extern constexpr const float isometric_y_scale = 1.154700538379252f; //1.0f / Math::Cos(Math::Radians(-30.0f));

    extern constexpr const float pixels_per_unit = 16.0f;
    extern constexpr const float units_per_pixel = 1.0f / pixels_per_unit;

    extern constexpr const float rotation_grid = 30.0f / 16.0f;
    extern constexpr const float inverse_rotation_grid = 1.0f / rotation_grid;

    // Generics
    extern inline constexpr float roundf(const float f);

    extern inline constexpr float absf(const float f);

    extern inline constexpr float fmodf(const float x, const float y);

    extern inline constexpr float fminf(const float a, const float b);
    extern inline constexpr float fmaxf(const float a, const float b);
    extern inline constexpr float clampf(const float f, const float min, const float max);

    extern inline constexpr float sqrtf(const float f);
    extern inline constexpr float rsqrtf(const float f);

    // Generics To-Int
    extern inline constexpr int ceilf_i(const float f);
    extern inline constexpr int floorf_i(const float f);
    extern inline constexpr int roundf_i(const float f);

    extern inline constexpr int ceilEvenf_i(const float f);
    extern inline constexpr int ceilOddf_i(const float f);

    // Geometry
    bool IntersectRayPlane(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, float& f) { return glm::intersectRayPlane(v1, v2, v3, v4, f); }

    // Interpolation
    extern inline constexpr float interpolationHermitef(const float t);
    extern inline constexpr float interpolationQuinticf(const float t);

    extern inline constexpr float lerpf(const float a, const float b, const float t);
    extern inline constexpr Vector3 lerpv3(const Vector3 a, const Vector3 b, const float t);
    extern inline constexpr float cubicLerpf(const float a, const float b, const float c, const float d, const float t);
    extern inline constexpr Vector3 cubicLerpv3(const Vector3 a, const Vector3 b, const Vector3 c, const Vector3 d, const float t);

    extern inline constexpr Vector3 smoothNormalFromFour(const float* heightMap, const float dx, const float dz, const unsigned int width, const unsigned int height, const unsigned int z, const unsigned int x);
    extern inline constexpr Vector3 smoothNormalFromEight(const float* heightMap, const float dx, const float dz, const unsigned int width, const unsigned int height, const unsigned int z, const unsigned int x);

    // Matrix
    typedef struct{
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
        constexpr Matrix3x3(const Vector3& v0, const Vector3& v1, const Vector3& v2);
        constexpr Matrix3x3(const Matrix4x4& m);

        constexpr Vector3& operator[](unsigned int i) noexcept;
        constexpr const Vector3& operator[](unsigned int i) const noexcept;
        constexpr Matrix3x3& operator=(const Matrix3x3& m);

    private:
        Vector3 value[3];
    }Matrix3x3;

    extern inline constexpr Matrix3x3 mat3Mult(const Matrix3x3& m, const float scalar);
    extern inline constexpr Vector3 mat3Mult(const Matrix3x3& m, const Vector3& v);
    extern inline constexpr Vector3 mat3Mult(const Vector3& v, const Matrix3x3& m);
    extern inline constexpr Matrix3x3 mat3Mult(const Matrix3x3& m1, const Matrix3x3& m2);
    extern inline constexpr Matrix3x3 mat3Inverse(const Matrix3x3& m);

    extern inline constexpr bool operator==(const Matrix3x3& m1, const Matrix3x3& m2);
    extern inline constexpr bool operator!=(const Matrix3x3& m1, const Matrix3x3& m2);

    typedef struct {
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
        constexpr Matrix4x4(const Vector4& v0, const Vector4& v1, const Vector4& v2, const Vector4& v3);
        constexpr Matrix4x4(const Matrix3x3& m);

        constexpr Vector4& operator[](unsigned int i) noexcept;
        constexpr const Vector4& operator[](unsigned int i) const noexcept;
        constexpr Matrix4x4& operator=(const Matrix4x4& m);

    private:
        Vector4 value[4];
    }Matrix4x4;

    extern inline constexpr Matrix4x4 mat4Mult(const Matrix4x4& m, const float scalar);
    extern inline constexpr Vector4 mat4Mult(const Matrix4x4& m, const Vector4& v);
    extern inline constexpr Vector4 mat4Mult(const Vector4& v, const Matrix4x4& m);
    extern inline constexpr Matrix4x4 mat4Mult(const Matrix4x4& m1, const Matrix4x4& m2);
    extern inline constexpr Matrix4x4 mat4Inverse(const Matrix4x4& m);

    extern inline constexpr bool operator==(const Matrix4x4& m1, const Matrix4x4& m2);
    extern inline constexpr bool operator!=(const Matrix4x4& m1, const Matrix4x4& m2);

    // Retro
    extern inline constexpr Vector3 roundVec3ToPixel(const Vector3 vector);
    extern inline constexpr Vector4 roundVec4ToPixel(const Vector4 vector);

    // Trigonometry
    extern inline constexpr float radiansf(const float degrees);
    extern inline constexpr float degreesf(const float radians);

    extern inline constexpr float clampDegreesf(const float degrees);
    extern inline constexpr float clampRadiansf(const float radians);

    extern inline constexpr float cosf(const float radians);
    extern inline constexpr float sinf(const float radians);
    extern inline constexpr float atanf(const float radians);
    extern inline constexpr float atan2f(const float y, const float x);

    // Vector
    typedef struct {
    public:
        static constexpr unsigned int length() { return 2u; }

        constexpr Vector2();
        constexpr Vector2(const Vector2& v);
        constexpr Vector2(const Vector3& v);
        constexpr Vector2(const Vector4& v);
        constexpr Vector2(const float x, const float y);

        constexpr float& operator[](unsigned int i) noexcept;
        constexpr const float& operator[](unsigned int i) const noexcept;
        constexpr Vector2& operator=(const Vector2& v);

    private:
        float x;
        float y;
    }Vector2;

    extern inline constexpr bool operator==(const Vector2& v1, const Vector2& v2);
    extern inline constexpr bool operator!=(const Vector2& v1, const Vector2& v2);

    extern inline constexpr float vec2Dot(const Vector2 v1, const Vector2 v2);
    extern inline constexpr Vector2 vec2Mul(const Vector2 v1, const Vector2 v2);
    extern inline constexpr Vector2 vec2Normalize(const Vector2& v);

    typedef struct {
    public:
        static constexpr unsigned int length() { return 3u; }

        constexpr Vector3();
        constexpr Vector3(const Vector3& v);
        constexpr Vector3(const Vector4& v);
        constexpr Vector3(const float x, const float y, const float z);

        constexpr float& operator[](unsigned int i) noexcept;
        constexpr const float& operator[](unsigned int i) const noexcept;
        constexpr Vector3& operator=(const Vector3& v);

    private:
        float x;
        float y;
        float z;
    }Vector3;

    extern inline constexpr bool operator==(const Vector3& v1, const Vector3& v2);
    extern inline constexpr bool operator!=(const Vector3& v1, const Vector3& v2);

    extern inline constexpr float vec3Dot(const Vector3 v1, const Vector3 v2);
    extern inline constexpr Vector3 vec3Mul(const Vector3 v1, const Vector3 v2);
    extern inline constexpr Vector3 vec3Normalize(const Vector3& v);
    extern inline constexpr Vector3 vec3Cross(const Vector3 v1, const Vector3 v2);

    typedef struct {
    public:
        static constexpr unsigned int length() { return 4u; }

        constexpr Vector4();
        constexpr Vector4(const Vector4& v);
        constexpr Vector4(const float x, const float y, const float z, const float w);

        constexpr float& operator[](unsigned int i) noexcept;
        constexpr const float& operator[](unsigned int i) const noexcept;
        constexpr Vector4& operator=(const Vector4& v);

    private:
        float x;
        float y;
        float z;
        float w;
    }Vector4;

    extern inline constexpr bool operator==(const Vector4& v1, const Vector4& v2);
    extern inline constexpr bool operator!=(const Vector4& v1, const Vector4& v2);

    extern inline constexpr float vec4Dot(const Vector4 v1, const Vector4 v2);
    extern inline constexpr Vector4 ve43Mul(const Vector4 v1, const Vector4 v2);
    extern inline constexpr Vector4 vec4Normalize(const Vector4& v);
}
#endif
