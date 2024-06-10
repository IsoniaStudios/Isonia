namespace Isonia::Math
{
    extern inline constexpr unsigned int randomInt(const unsigned int seed)
    {
        seed = (seed << 13) ^ seed;
        return ((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff);
    }
}
