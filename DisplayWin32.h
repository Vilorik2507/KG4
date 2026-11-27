#pragma once

#include <Windows.h>
#include "InputDevice.h"

class DisplayWin32
{
public:
    DisplayWin32() = default;
    ~DisplayWin32();

    DisplayWin32(const DisplayWin32&) = delete;
    DisplayWin32& operator=(const DisplayWin32&) = delete;

    //инациализация окна
    bool Initialize(
        HINSTANCE instance,
        int width,
        int height,
        const wchar_t* title
    );

    //сообщения
    void ProcessMessages();

    //контроль окна
    void Show();
    void Close();

    //состояния окна
    bool IsRunning() const noexcept;
    bool IsMinimiz() const noexcept;

    
    HWND GetHandle() const noexcept;

    int GetClientWidth() const noexcept;
    int GetClientHeight() const noexcept;

    //ввод с клавиатуры или мыши
    InputDevice& GetInput() noexcept;

private:
    
    static LRESULT CALLBACK WindowProc(
        HWND hwnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );

    LRESULT HandleMessage(
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );

    bool RegisterWindowClass();

private:
    HINSTANCE instance_ = nullptr;
    HWND window_ = nullptr;

    InputDevice input_;

    int clientWidth_ = 0;
    int clientHeight_ = 0;

    bool running_ = false;
    bool minimized_ = false;

    const wchar_t* windowTitle_ = L"DirectX12 Framework";

    static constexpr const wchar_t* WindowClassName =
        L"DirectX12FrameworkWindow";
};

