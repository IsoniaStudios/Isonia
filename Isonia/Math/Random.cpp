// internal
#include "Math.h"

namespace Isonia::Math
{
    extern constexpr unsigned int randomInt(const unsigned int seed)
    {
        const unsigned int hash = (seed << 13) ^ seed;
        return ((hash * (hash * hash * 15731 + 789221) + 1376312589) & 0x7fffffff);
    }
}
