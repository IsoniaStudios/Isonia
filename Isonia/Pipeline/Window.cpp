// internal
#include "Pipeline.h"

// external
#include <windows.h>
#include <vulkan/vulkan_win32.h>

namespace Isonia::Pipeline
{
    Window::Window(const unsigned int width, const unsigned int height, const char* name)
        : m_extent({ width, height }), m_name(name)
    {
        createWindow();
    }

    Window::~Window()
    {
    }

    int Window::getKey(int key) const
    {
        return 0;
    }

    void Window::pollEvents()
    {
        MSG message;

        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                m_should_close = true;
            }
            else
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
    }

    void Window::waitEvents() const
    {

    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = static_cast<HWND>(m_window);
        createInfo.hinstance = static_cast<HINSTANCE>(m_window_instance);

        if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
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

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_NCCREATE:
        {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreate->lpCreateParams));
        }
        break;

        case WM_DESTROY:
        {
            DestroyWindow(hWnd);
            PostQuitMessage(0);
        }
        break;

        case WM_PAINT:
        {
            ValidateRect(hWnd, NULL);
        }
        break;

        case WM_SIZE:
        {
            const unsigned int width = LOWORD(lParam);
            const unsigned int height = HIWORD(lParam);

            Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            if (window)
            {
                window->m_resized = true;
                window->m_extent = { width, height };
                window->propagateEvent();
            }
        }
        break;
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }


    void Window::createWindow()
    {
        m_window_instance = GetModuleHandle(NULL);

        AllocConsole();
        AttachConsole(GetCurrentProcessId());
        freopen("CON", "w", stdout);
        freopen("CON", "w", stderr);
        SetConsoleTitle(TEXT(m_name));

        HINSTANCE h_instance_recast = static_cast<HINSTANCE>(m_window_instance);
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = h_instance_recast;
        wcex.hIcon = LoadIcon(h_instance_recast, MAKEINTRESOURCE(IDI_APPLICATION));
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = m_name;
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

        if (!RegisterClassEx(&wcex))
        {
            throw std::runtime_error("Failed to register window");
        }

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        int windowX = screenWidth / 2 - m_extent.width / 2;
        int windowY = screenHeight / 2 - m_extent.height / 2;

        RECT desiredRect = { 0, 0, m_extent.width, m_extent.height };
        AdjustWindowRect(&desiredRect, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, FALSE);
        int windowWidth = desiredRect.right - desiredRect.left;
        int windowHeight = desiredRect.bottom - desiredRect.top;

        m_window = CreateWindow(m_name, m_name, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, windowX, windowY, windowWidth, windowHeight, NULL, NULL, h_instance_recast, this);

        HWND m_window_recast = static_cast<HWND>(m_window);
        ShowWindow(m_window_recast, SW_SHOW);
        SetForegroundWindow(m_window_recast);
        SetFocus(m_window_recast);
    }
}
