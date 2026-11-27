
#include <Windows.h>

#include "DisplayWin32.h"

int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PWSTR pCmdLine,
    int nCmdShow)
{
    //создаем окно
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
            MB_OK | MB_ICONERROR
        );

        return -1;
    }

    //выводим
    display.Show();

    //сообщения
    while (display.IsRunning())
    {
        display.ProcessMessages();

        if (display.IsMinimiz())
        {
            Sleep(10);
            continue;
        }

        //обновление
        display.GetInput().Update();
    }

    return 0;
}
