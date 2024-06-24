// internal
#include "Pipeline.h"

// external
#include <windows.h>

LRESULT CALLBACK WindowProcessMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_QUIT:
    case WM_DESTROY: {
    } break;

    default: { // Message not handled; pass on to default message handling function
        return DefWindowProc(window_handle, message, wParam, lParam);
    } break;
    }
    return 0;
}

namespace Isonia::Pipeline
{
    Window::Window(const unsigned int width, const unsigned int height, const char* name)
        : m_extent({ width, height }), m_name(name)
    {
        WNDCLASS window_class = { 0 };
        window_class.lpszClassName = name;
        window_class.lpfnWndProc = WindowProcessMessage;

        RegisterClass(&window_class);

        HWND window_handle = CreateWindow(name, "test", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);
        if (window_handle == NULL)
        {
            throw std::runtime_error("Failed to create window handle!");
        }

        ShowWindow(window_handle, SW_SHOW);
    }

    Window::~Window()
    {
    }

    int Window::getKey(int key) const
    {
        return 0;
    }

    bool Window::shouldClose() const
    {
        return false;
    }

    void Window::pollEvents() const
    {

    }

    void Window::waitEvents() const
    {

    }

    const VkExtent2D Window::getExtent() const
    {
        return m_extent;
    }

    void Window::resetResizedFlag()
    {
        m_resized = false;
    }

    const bool Window::wasResized() const
    {
        return m_resized;
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {

    }

    void Window::registerCallback(EventHandler handler)
    {
        m_handlers[m_event_count++] = handler;
    }

    void Window::propagateEvent()
    {
        for (unsigned int i = 0; i < m_event_count; i++)
        {
            m_handlers[i](this);
        }
    }

    void Window::framebufferResizeCallback(const unsigned int width, const unsigned int height)
    {
        m_resized = true;
        m_extent = { width, height };
        propagateEvent();
    }
}
