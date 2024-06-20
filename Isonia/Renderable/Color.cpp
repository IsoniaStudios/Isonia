// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	constexpr Color::Color(const unsigned char r, const unsigned char g, const unsigned char b)
		: r(r), g(g), b(b), a(255)
	{
	}
	constexpr Color::Color(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
		: r(r), g(g), b(b), a(a)
	{
	}
	constexpr Color::Color()
	{
	}
}
