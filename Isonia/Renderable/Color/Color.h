#pragma once

// std
#include <cstdint>

namespace Isonia::Renderable::Color
{
	struct Color
	{
	public:
		constexpr Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), a(255)
		{
		}
		constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a)
		{
		}
		constexpr Color()
		{
		}

		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	private:
	};
}
