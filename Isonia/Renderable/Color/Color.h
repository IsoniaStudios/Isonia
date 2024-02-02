#pragma once

#include <memory>

namespace Isonia::Renderable::Color
{
	struct Color
	{
	public:
		Color(std::uint8_t r, std::uint8_t g, std::uint8_t b) : r(r), g(g), b(b), a(255)
		{

		}
		Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) : r(r), g(g), b(b), a(a)
		{

		}

		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;
		std::uint8_t a;
	private:
	};
}
