#include "Window.h"

// std
#include <stdexcept>

namespace Isonia::Window
{
	Window::Window(int width, int height, std::string name) : width(width), height(height), name(name)
	{
		InitWindow();
	}
	Window::~Window()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	VkExtent2D Window::GetExtent()
	{
		return {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};
	}
	bool Window::WasWindowResized()
	{
		return framebufferResized;
	}
	void Window::ResetWindowResizedFlag()
	{
		framebufferResized = false;
	}

	void Window::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
	}

	void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to craete window surface");
		}
	}

	void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto localWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		localWindow->framebufferResized = true;
		localWindow->width = width;
		localWindow->height = height;
	}
}
