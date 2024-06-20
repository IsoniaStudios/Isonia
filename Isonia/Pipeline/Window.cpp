// internal
#include "Pipeline.h"

namespace Isonia::Pipeline
{
    Window::Window(const unsigned int width, const unsigned int height, const char* name)
        : m_width(width), m_height(height), m_name(name)
    {
        initWindow();
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }

    const VkExtent2D Window::getExtent() const
    {
        return { static_cast<unsigned int>(m_width), static_cast<unsigned int>(m_height) };
    }

    void Window::resetWindowResizedFlag()
    {
        m_framebuffer_resized = false;
    }

    const bool Window::wasWindowResized() const
    {
        return m_framebuffer_resized;
    }

    GLFWwindow* Window::getGLFWWindow() const
    {
        return m_window;
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface");
        }
    }

    void Window::registerCallback(EventHandler handler)
    {
        m_handlers[m_handlers_count++] = handler;
    }

    void Window::propagateEvent()
    {
        for (unsigned int i = 0; i < m_handlers_count; i++)
        {
            m_handlers[i](this);
        }
    }

    void Window::initWindow()
    {
        glfwInit();
#ifdef DLL_BUILD
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
#else
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
#endif

        m_window = glfwCreateWindow(m_width, m_height, m_name, nullptr, nullptr);
        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
    }

    void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        Window* local_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        local_window->m_framebuffer_resized = true;
        local_window->m_width = width;
        local_window->m_height = height;
        local_window->propagateEvent();
    }
}
