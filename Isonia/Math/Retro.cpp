// internal
#include "Math.h"

namespace Isonia::Math
{
    extern constexpr Vector3 roundVec3ToPixel(const Vector3 vector)
    {
        return Vector3{
            roundf(vector.x * pixels_per_unit) * units_per_pixel,
            roundf(vector.y * pixels_per_unit) * units_per_pixel,
            roundf(vector.z * pixels_per_unit) * units_per_pixel
        };
    }
    extern constexpr Vector4 roundVec4ToPixel(const Vector4 vector)
    {
        return Vector4{
            roundf(vector.x * pixels_per_unit) * units_per_pixel,
            roundf(vector.y * pixels_per_unit) * units_per_pixel,
            roundf(vector.z * pixels_per_unit) * units_per_pixel,
            vector.w
        };
    }
}
