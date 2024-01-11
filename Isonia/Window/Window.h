#pragma once

// glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std
#include <string>
#include <stdexcept>

namespace Isonia::Window
{
	class Window
	{
	public:
		Window(int width, int height, std::string name) : width(width), height(height), name(name)
		{
			InitWindow();
		}
		~Window()
		{
			glfwDestroyWindow(window);
			glfwTerminate();
		}

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		bool ShouldClose() const
		{
			return glfwWindowShouldClose(window);
		}

		const VkExtent2D GetExtent() const
		{
			return {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};
		}

		void ResetWindowResizedFlag()
		{
			framebufferResized = false;
		}

		const bool WasWindowResized() const
		{
			return framebufferResized;
		}

		GLFWwindow* GetGLFWwindow() const
		{
			return window;
		}

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
		{
			if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to craete window surface");
			}
		}

	private:
		void InitWindow()
		{
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

			window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
			glfwSetWindowUserPointer(window, this);
			glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
		}

		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height)
		{
			auto localWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
			localWindow->framebufferResized = true;
			localWindow->width = width;
			localWindow->height = height;
		}

		int width;
		int height;
		bool framebufferResized = false;

		const std::string name;
		GLFWwindow* window;
	};
}
