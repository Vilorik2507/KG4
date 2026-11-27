
#include "DisplayWin32.h"

#include <windowsx.h>

DisplayWin32::~DisplayWin32()
{
    if (window_ && IsWindow(window_))
    {
        DestroyWindow(window_);
    }
}

bool DisplayWin32::Initialize(
    HINSTANCE instance,
    int width,
    int height,
    const wchar_t* title)
{
    if (window_ != nullptr)
        return false;

    if (width <= 0 || height <= 0 || title == nullptr)
        return false;

    instance_ = instance;
    clientWidth_ = width;
    clientHeight_ = height;
    windowTitle_ = title;

    if (!RegisterWindowClass())
        return false;

    RECT windowRect =
    {
        0,
        0,
        width,
        height
    };

    DWORD windowStyle = WS_OVERLAPPEDWINDOW;

    if (!AdjustWindowRect(
        &windowRect,
        windowStyle,
        FALSE))
    {
        return false;
    }

    window_ = CreateWindowEx(
        0,
        WindowClassName,
        windowTitle_,
        windowStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        instance_,
        this
    );

    if (!window_)
        return false;

    running_ = true;

    return true;
}

bool DisplayWin32::RegisterWindowClass()
{
    WNDCLASSEX wc{};

    wc.cbSize = sizeof(WNDCLASSEX);

    wc.style = CS_HREDRAW | CS_VREDRAW;

    wc.lpfnWndProc = WindowProc;

    wc.hInstance = instance_;

    wc.hCursor = LoadCursor(
        nullptr,
        IDC_ARROW
    );

    wc.hbrBackground = nullptr;

    wc.lpszClassName = WindowClassName;

    if (!RegisterClassEx(&wc))
    {
        if (GetLastError() == ERROR_CLASS_ALREADY_EXISTS)
            return true;

        return false;
    }

    return true;
}

void DisplayWin32::Show()
{
    if (window_)
    {
        ShowWindow(window_, SW_SHOW);
        UpdateWindow(window_);
    }
}

void DisplayWin32::ProcessMessages()
{
    MSG message{};

    while (PeekMessage(
        &message,
        nullptr,
        0,
        0,
        PM_REMOVE))
    {
        if (message.message == WM_QUIT)
        {
            running_ = false;
            return;
        }

        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

void DisplayWin32::Close()
{
    if (window_ && IsWindow(window_))
    {
        DestroyWindow(window_);
    }

    running_ = false;
}

bool DisplayWin32::IsRunning() const noexcept
{
    return running_;
}

bool DisplayWin32::IsMinimiz() const noexcept
{
    return minimized_;
}

HWND DisplayWin32::GetHandle() const noexcept
{
    return window_;
}

int DisplayWin32::GetClientWidth() const noexcept
{
    return clientWidth_;
}

int DisplayWin32::GetClientHeight() const noexcept
{
    return clientHeight_;
}

InputDevice& DisplayWin32::GetInput() noexcept
{
    return input_;
}


LRESULT CALLBACK DisplayWin32::WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    DisplayWin32* window = nullptr;

    if (message == WM_NCCREATE)
    {
        const CREATESTRUCT* createStruct =
            reinterpret_cast<CREATESTRUCT*>(lParam);

        window = static_cast<DisplayWin32*>(
            createStruct->lpCreateParams
            );

        SetWindowLongPtr(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(window)
        );

        window->window_ = hwnd;
    }
    else
    {
        window = reinterpret_cast<DisplayWin32*>(
            GetWindowLongPtr(hwnd, GWLP_USERDATA)
            );
    }

    if (window)
    {
        return window->HandleMessage(
            message,
            wParam,
            lParam
        );
    }

    return DefWindowProc(
        hwnd,
        message,
        wParam,
        lParam
    );
}


LRESULT DisplayWin32::HandleMessage(
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
    {
        DestroyWindow(window_);
        return 0;
    }

    case WM_DESTROY:
    {
        running_ = false;
        window_ = nullptr;

        PostQuitMessage(0);

        return 0;
    }

    case WM_SIZE:
    {
        clientWidth_ = LOWORD(lParam);
        clientHeight_ = HIWORD(lParam);

        minimized_ = (wParam == SIZE_MINIMIZED);

        return 0;
    }

    //ввод с клавиатцры
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        input_.OnKeyDown(wParam);
        break;
    }

    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        input_.OnKeyUp(wParam);
        break;
    }

    //ввод с мыши
    case WM_MOUSEMOVE:
    {
        input_.OnMouseMove(
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam)
        );

        break;
    }

    //ввод с мыши кнопки
    case WM_LBUTTONDOWN:
    {
        input_.OnMouseButtonDown(0);
        SetCapture(window_);
        break;
    }

    case WM_LBUTTONUP:
    {
        input_.OnMouseButtonUp(0);
        ReleaseCapture();
        break;
    }

    case WM_RBUTTONDOWN:
    {
        input_.OnMouseButtonDown(1);
        break;
    }

    case WM_RBUTTONUP:
    {
        input_.OnMouseButtonUp(1);
        break;
    }

    case WM_MBUTTONDOWN:
    {
        input_.OnMouseButtonDown(2);
        break;
    }

    case WM_MBUTTONUP:
    {
        input_.OnMouseButtonUp(2);
        break;
    }

    //ввод с мыши колесико
    case WM_MOUSEWHEEL:
    {
        input_.OnMouseWheel(
            GET_WHEEL_DELTA_WPARAM(wParam)
        );

        break;
    }

    case WM_KILLFOCUS:
    {
        input_.Reset();
        break;
    }
    }

    return DefWindowProc(
        window_,
        message,
        wParam,
        lParam
    );
}
