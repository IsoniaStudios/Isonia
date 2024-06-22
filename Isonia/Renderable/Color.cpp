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

	extern Texture* createGrassDayPalette(Pipeline::Device* device)
	{
		constexpr const Color grass_day_palette[palette_length] =
		{
			{ 56, 55, 77 },
			{ 60, 63, 78 },
			{ 74, 91, 88 },
			{ 76, 105, 81 },
			{ 104, 145, 101 },
			{ 132, 191, 110 },
			{ 157, 220, 114 },
			{ 184, 232, 120 },
			{ 195, 239, 126 },
			{ 218, 250, 139 },
		};
		return Texture::createTextureFromPalette(device, grass_day_palette, palette_length);
	}

	extern Texture* createGrassNightPalette(Pipeline::Device* device)
	{
		constexpr const Color grass_night_palette[palette_length] =
		{
			{ 0, 14, 28 },
			{ 1, 18, 32 },
			{ 2, 22, 35 },
			{ 4, 30, 45 },
			{ 16, 46, 65 },
			{ 24, 63, 90 },
			{ 33, 89, 119 },
			{ 51, 115, 145 },
			{ 86, 154, 181 },
			{ 102, 173, 198 },
		};
		return Texture::createTextureFromPalette(device, grass_night_palette, palette_length);
	}

	extern Texture* createWaterDayPalette(Pipeline::Device* device)
	{
		constexpr const Color water_day_palette[palette_length] =
		{
			{ 187, 222, 229 },
			{ 178, 221, 230 },
			{ 157, 214, 226 },
			{ 133, 209, 226 },
			{ 88, 190, 219 },
			{ 82, 190, 222 },
			{ 64, 177, 212 },
			{ 44, 146, 184 },
			{ 15, 101, 138 },
			{ 16, 81, 123 },
		};
		return Texture::createTextureFromPalette(device, water_day_palette, palette_length);
	}
}
