#pragma once

#include "Window/Window.h"

namespace Isonia
{
	class Isonia
	{
	public:
		static constexpr int WIDTH = 1024;
		static constexpr int HEIGHT = 576;
		static constexpr const char* NAME = "Window";

		void Run();

	private:
		Window::Window window { WIDTH, HEIGHT, NAME };
	};
}
