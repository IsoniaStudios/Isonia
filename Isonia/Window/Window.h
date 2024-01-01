#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace Isonia::Window
{
	class Window
	{
	public:
		Window(int width, int height, std::string name);
		~Window();

		bool ShouldClose();

	private:
		void InitWindow();

		const int width;
		const int height;

		const std::string name;
		GLFWwindow* window;
	};
}
