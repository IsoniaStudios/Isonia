#pragma once

// libraries
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std
#include <string>

namespace Isonia::Window
{
	class Window
	{
	public:
		Window(int width, int height, std::string name);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		bool ShouldClose();

		VkExtent2D GetExtent();
		bool WasWindowResized();
		void ResetWindowResizedFlag();
		GLFWwindow* GetGLFWwindow() const { return window; }

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		void InitWindow();

		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

		int width;
		int height;
		bool framebufferResized = false;

		const std::string name;
		GLFWwindow* window;
	};
}
