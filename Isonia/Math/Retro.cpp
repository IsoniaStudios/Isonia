namespace Isonia::Math
{
    extern inline constexpr Vector3 roundVec3ToPixel(const Vector3 vector)
    {
        return Vector3{
            roundf(vector.x * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            roundf(vector.y * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            roundf(vector.z * PIXELS_PER_UNIT) * UNITS_PER_PIXEL
        };
    }

    extern inline constexpr Vector4 roundVec4ToPixel(const Vector4 vector)
    {
        return Vector4{
            roundf(vector.x * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            roundf(vector.y * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            roundf(vector.z * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            vector.w
        };
    }
}
