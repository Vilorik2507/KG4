
#include <Windows.h>

#include "DisplayWin32.h"
#include "Timer.h"
#include "Graphics.h"
//инициализируем экран
int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PWSTR pCmdLine,
    int nCmdShow)
{
    DisplayWin32 display;

    if (!display.Initialize(
        hInstance,
        1280,
        720,
        L"DirectX 12 Framework"))
    {
        MessageBox(
            nullptr,
            L"Failed to initialize window.",
            L"Initialization Error",
            MB_OK | MB_ICONERROR);

        return -1;
    }
    //выводим экран
    display.Show();

    Graphics graphics;

    if (!graphics.Initialize(
        display.GetHandle(),
        display.GetClientWidth(),
        display.GetClientHeight()))
    {
        MessageBox(
            display.GetHandle(),
            L"Failed to initialize DirectX 12.",
            L"Graphics Error",
            MB_OK | MB_ICONERROR);

        return -1;
    }

    Timer timer;
    timer.Reset();

    while (display.IsRunning())
    {
        display.ProcessMessages();

        if (!display.IsRunning())
            break;

        if (display.IsMinimiz())
        {
            Sleep(10);
            continue;
        }

        timer.Tick();

        //обработка пользовательского ввода
        display.GetInput().Update();

        //очистка Back Buffer
        graphics.Clear();

        //вывод кадра
        graphics.Present();
    }

    graphics.WaitForGPU();

    return 0;
}
