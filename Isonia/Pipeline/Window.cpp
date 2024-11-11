// internal
#include "Pipeline.h"

// external
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#include <stdexcept>
#include <iostream>

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

    unsigned char Window::getKey(unsigned int key) const
    {
        return m_input.current_key_state[key];
    }

    void Window::pollEvents()
    {
        // switch current and previous
        memcpy(m_input.previous_key_state, m_input.current_key_state, sizeof(m_input.current_key_state));

        MSG message;
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
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
        VkWin32SurfaceCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        create_info.hwnd = static_cast<HWND>(m_window);
        create_info.hinstance = static_cast<HINSTANCE>(m_window_instance);

        if (vkCreateWin32SurfaceKHR(instance, &create_info, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface!");
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

    void Window::inputKey(unsigned int key, unsigned char action)
    {
        if (key < State::Keyboard::max_keyboard_keys)
        {
            m_input.current_key_state[key] = action;
        }
    }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_NCCREATE:
        {
            CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
        }
        break;

        case WM_DESTROY:
        {
            Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            window->m_should_close = true;
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
            const VkExtent2D old_extent = window->m_extent;
            if (width == old_extent.width && height == old_extent.height)
            {
                break;
            }
            window->m_extent = { width, height };
            window->m_resized = true;
            window->propagateEvent();
        }
        break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            const char action = (HIWORD(lParam) & KF_UP) ? KeyActions::release : KeyActions::press;

            int key = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
            if (!key)
            {
                key = MapVirtualKeyW((UINT)wParam, MAPVK_VK_TO_VSC);
            }

            if (key == 0x54)
            {
                key = 0x137;
            }
            else if (key == 0x146)
            {
                key = 0x45;
            }
            else if (key == 0x136)
            {
                key = 0x36;
            }

            // The Ctrl keys require special handling
            if (wParam == VK_CONTROL)
            {
                if (HIWORD(lParam) & KF_EXTENDED)
                {
                    key = KeyCodes::right_control;
                }
                else
                {
                    MSG next;
                    const DWORD time = GetMessageTime();

                    if (PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE))
                    {
                        if (next.message == WM_KEYDOWN || next.message == WM_SYSKEYDOWN || next.message == WM_KEYUP || next.message == WM_SYSKEYUP)
                        {
                            if (next.wParam == VK_MENU && (HIWORD(next.lParam) & KF_EXTENDED) && next.time == time)
                            {
                                break;
                            }
                        }
                    }
                    key = KeyCodes::left_control;
                }
            }
            else if (wParam == VK_PROCESSKEY)
            {
                break;
            }

            Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            if (action == KeyActions::release && wParam == VK_SHIFT)
            {
                window->inputKey(KeyCodes::left_shift, action);
                window->inputKey(KeyCodes::right_shift, action);
            }
            else if (wParam == VK_SNAPSHOT)
            {
                window->inputKey(key, KeyActions::press);
                window->inputKey(key, KeyActions::release);
            }
            else
            {
                window->inputKey(key, action);
            }
        }
        break;
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }


    void Window::createWindow()
    {
        HANDLE h_process = GetCurrentProcess();
        if (!SetPriorityClass(h_process, HIGH_PRIORITY_CLASS))
        {
            std::cerr << "Failed to set process priority." << '\n';
        }

        m_window_instance = GetModuleHandle(NULL);

        HINSTANCE h_instance_recast = static_cast<HINSTANCE>(m_window_instance);
        WNDCLASSEX wcex;
        
#if DEBUG
        AllocConsole();
        AttachConsole(GetCurrentProcessId());
        FILE* stream_out = freopen("CON", "w", stdout);
        FILE* stream_err = freopen("CON", "w", stderr);
        SetConsoleTitle(TEXT(m_name));
#endif
        
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = h_instance_recast;
        wcex.lpszClassName = m_name;
        wcex.lpszMenuName = NULL;
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
        wcex.hIcon = LoadIcon(h_instance_recast, MAKEINTRESOURCE(IDI_APPLICATION));
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);

        if (!RegisterClassEx(&wcex))
        {
            throw std::runtime_error("Failed to register window");
        }

        int screen_width = GetSystemMetrics(SM_CXSCREEN);
        int screen_height = GetSystemMetrics(SM_CYSCREEN);
        int window_x = screen_width / 2 - m_extent.width / 2;
        int window_y = screen_height / 2 - m_extent.height / 2;

        RECT desired_rect = { 0, 0, static_cast<LONG>(m_extent.width), static_cast<LONG>(m_extent.height) };
        AdjustWindowRect(&desired_rect, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, FALSE);
        int window_width = desired_rect.right - desired_rect.left;
        int window_height = desired_rect.bottom - desired_rect.top;

        m_window = CreateWindow(m_name, m_name, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, window_x, window_y, window_width, window_height, NULL, NULL, h_instance_recast, this);

        HWND m_window_recast = static_cast<HWND>(m_window);
        ShowWindow(m_window_recast, SW_SHOW);
        SetForegroundWindow(m_window_recast);
        SetFocus(m_window_recast);
    }
}
