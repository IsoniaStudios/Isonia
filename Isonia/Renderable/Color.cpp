// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	extern Color blendColors(const Color* colors, const unsigned int color_count)
	{
		unsigned int totalR = 0u;
		unsigned int totalG = 0u;
		unsigned int totalB = 0u;
		unsigned int totalA = 0u;

		for (unsigned int i = 0u; i < color_count; i++)
		{
			totalR += colors[i].r;
			totalG += colors[i].g;
			totalB += colors[i].b;
			totalA += colors[i].a;
		}

		unsigned char avgR = static_cast<unsigned char>(totalR / color_count);
		unsigned char avgG = static_cast<unsigned char>(totalG / color_count);
		unsigned char avgB = static_cast<unsigned char>(totalB / color_count);
		unsigned char avgA = static_cast<unsigned char>(totalA / color_count);

		return Color(avgR, avgG, avgB, avgA);
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
