#pragma once

// std
#include <memory>

namespace Isonia::Renderable::Color
{
	struct Color
	{
	public:
		constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b) : r(r), g(g), b(b), a(255)
		{

		}
		constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) : r(r), g(g), b(b), a(a)
		{

		}

		const std::uint8_t r;
		const std::uint8_t g;
		const std::uint8_t b;
		const std::uint8_t a;
	private:
	};
}
