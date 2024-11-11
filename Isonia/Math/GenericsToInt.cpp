// internal
#include "Math.h"

namespace Isonia::Math
{
    extern constexpr int ceilf_i(const float f)
    {
        if (f >= 0)
        {
            return static_cast<int>(f + 0.999999f);
        }
        return static_cast<int>(f);
    }
    extern constexpr int floorf_i(const float f)
    {
        if (f >= 0)
        {
            return static_cast<int>(f);
        }
        return static_cast<int>(f) - 1;
    }
    extern constexpr int roundf_i(const float f)
    {
        if (f >= 0.0f)
        {
            return static_cast<int>(f + 0.5f);
        }
        return static_cast<int>(f - 0.5f);
    }

    extern constexpr int ceilEvenf_i(const float f)
    {
        const int ceiled_f = ceilf_i(f);
        if (ceiled_f % 2 != 0)
        {
            return ceiled_f + 1;
        }
        return ceiled_f;
    }
    extern constexpr int ceilOddf_i(const float f)
    {
        const int ceiled_f = ceilf_i(f);
        if (ceiled_f % 2 == 0)
        {
            return ceiled_f + 1;
        }
        return ceiled_f;
    }
}
