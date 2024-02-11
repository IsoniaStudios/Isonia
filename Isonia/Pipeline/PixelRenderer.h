#pragma once

// internal
#include "Renderer.h"

namespace Isonia::Pipeline
{
	class PixelRenderer : public Renderer
	{
	public:
		PixelRenderer(Window::Window& window, Device& device, std::uint32_t width, std::uint32_t height, std::uint8_t width_scale, std::uint8_t height_scale) : Renderer(window, device)
		{
		}

		~PixelRenderer()
		{
		}

		// Sets the size of the rendering surface
		void SetSize(std::uint32_t width, std::uint32_t height)
		{
			this->width = width;
			this->height = height;
		}

		// Set the array/pointer from which to obtain the pixels from
		void SetPixels(std::uint8_t* pixels)
		{
			this->pixels = pixels;
		}

	private:
		// The intended native (non-upscaled) rendering resolution
		std::uint32_t width;
		std::uint32_t height;

		// Pointer to an array of pixels for rendering.
		// Array size without colour width * height, with colour width * height * 3.
		std::uint8_t* pixels;

		// How many times the result should be upscaled in the given directions
		std::uint8_t width_scale;
		std::uint8_t height_scale;

		// For storing the client size of the rendering surface
		std::uint32_t surface_width;
		std::uint32_t surface_height;

		// This function handles changes that need to be done when the surface area changes
		void OnResize()
		{
			width_scale = surface_width / width;
			height_scale = surface_height / height;
		}
	};
}