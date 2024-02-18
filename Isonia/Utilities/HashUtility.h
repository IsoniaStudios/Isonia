#pragma once

// std
#include <functional>

namespace Isonia::Utilities::Hash
{
	template <typename T, typename... Rest>
	void HashCombine(size_t& seed, const T& v, const Rest&... rest)
	{
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(HashCombine(seed, rest), ...);
	};

	constexpr uint32_t fnv1a_32(char const* s, size_t count)
	{
		return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
	}

	constexpr uint32_t operator "" _hash(char const* s, size_t count)
	{
		return fnv1a_32(s, count);
	}
}
